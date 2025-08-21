/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : FreeRTOS applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_freertos.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "queue.h"
#include "semphr.h"
#include "dcmi.h"

#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "../gui_guider.h"
#include "../events_init.h"
#include "custom.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    uint8_t buffer_index;
    size_t length;
} ImageQueueItem;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_COUNT 3       // 3 frame buffers
#define BUFFER_SIZE 1024*10  // 10KB per buffer
#define MAX_IMAGE_SIZE (BUFFER_SIZE * sizeof(uint32_t))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern volatile uint8_t g_MQTT_Data_Ready; // MQTT data ready flag
extern struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct;

uint32_t JpegBuffers[BUFFER_COUNT][BUFFER_SIZE] = {0};  // Image buffer array
ImageQueueItem xImageItem;

QueueHandle_t xImageQueue = NULL;       // Image queue handle
SemaphoreHandle_t xBufferSemaphore = NULL;  // Buffer semaphore

osThreadId_t ov2640TaskHandle;
osSemaphoreId_t sem_TakePhoto;
osSemaphoreId_t sem_GetPhoto;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
void vTask1(void *argument);
void vTask2(void *argument);
void vTask3(void *argument);
void vTask4(void *argument);

void vPrintString(const char *pcString)
{
    // Enter critical section
    taskENTER_CRITICAL();
    {
        printf("%s", pcString);
    }
    // Exit critical section
    taskEXIT_CRITICAL();
}
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t Task1Handle;
const osThreadAttr_t Task1_attributes = {
  .name = "Task1",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 1
};
osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .priority = (osPriority_t) osPriorityAboveNormal,  // Higher priority than Task4
  .stack_size = 1024 * 8
};
osThreadId_t Task3Handle;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 1
};
osThreadId_t Task4Handle;
const osThreadAttr_t Task4_attributes = {
    .name = "Task4",
    .priority = (osPriority_t) osPriorityNormal,
    .stack_size = 1024 * 24 
};

/* USER CODE END FunctionPrototypes */

/* USER CODE BEGIN 4 */
void vApplicationStackOverflowHook(xTaskHandle xTask, char *pcTaskName)
{
  printf("Error: Stack overflow in task %s\n", pcTaskName);
  while(1);
   /* Run time stack overflow checking is performed if
   configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2. This hook function is
   called if a stack overflow is detected. */
}
/* USER CODE END 4 */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  sem_TakePhoto = osSemaphoreNew(1, 0, NULL);
  sem_GetPhoto = osSemaphoreNew(1, 0, NULL);
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  // Create image queue
  xImageQueue = xQueueCreate(BUFFER_COUNT, sizeof(ImageQueueItem));
  if(xImageQueue == NULL)
  {
      printf("[ERROR] Failed to create image queue\n");
  }
  
  // Create buffer semaphore
  xBufferSemaphore = xSemaphoreCreateCounting(BUFFER_COUNT, BUFFER_COUNT);
  if(xBufferSemaphore == NULL)
  {
      printf("[FATAL] Failed to create buffer semaphore\n");
  }
  else
  {
      printf("[INIT] Buffer semaphore created with count: %lu\n", 
             uxSemaphoreGetCount(xBufferSemaphore));
  }
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  //Task1Handle = osThreadNew(vTask1, NULL, &Task1_attributes);
  Task2Handle = osThreadNew(vTask2, NULL, &Task2_attributes);
  //Task3Handle = osThreadNew(vTask3, NULL, &Task3_attributes);
  Task4Handle = osThreadNew(vTask4, NULL, &Task4_attributes);


  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}
/* USER CODE BEGIN Header_StartDefaultTask */
/**
* @brief Function implementing the defaultTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN defaultTask */
  /* Infinite loop */
  for(;;)
  {
    lv_tick_inc(1);
    osDelay(1);
  }
  /* USER CODE END defaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vTask1(void *argument)
{
  for( ; ; )
  {
      vPrintString("");
      /* Delay for a while */
      vTaskDelay(pdMS_TO_TICKS(300));
      //BSP_SHT20_GetData();
  }
}

void vTask2(void *argument)
{
    ImageQueueItem queue_item;
    uint32_t ulNotificationValue = 0;
    
    for( ; ; )
    {
        // Receive image buffer index from queue
        if (xQueueReceive(xImageQueue, &queue_item, portMAX_DELAY) == pdPASS)
        {
            // Update LVGL image display
            //camera_img_dsc.data = (uint8_t *)JpegBuffers[queue_item.buffer_index];
            //lv_img_set_src(camera_img, &camera_img_dsc);
            //lv_obj_refresh_ext_draw_size(camera_img);
            //for(int i = 0;i < 1024;i ++)
            //printf("%08x",JpegBuffers[queue_item.buffer_index][i]);
            //printf("\n\n\n");
           
            // Verify and release buffer
            if (xSemaphoreGive(xBufferSemaphore) != pdTRUE)
            {
                printf("[ERROR] Failed to release buffer semaphore\n");
            }
        }
        else
        {
            printf("[ERROR] Failed to receive from queue\n");
        }

        // LCD refresh
        lv_task_handler();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}



void vTask3(void *argument)
{
  for( ; ; )
  {
      vPrintString("");
      // ESP8266 processing
      vTaskDelay(pdMS_TO_TICKS(100));
      //printf("g_MQTT_Data_Ready:%d\n",g_MQTT_Data_Ready);
    if (g_MQTT_Data_Ready)
      {
        // Copy data to local buffer
        char temp_buf[RX_BUF_MAX_LEN];
        strncpy(temp_buf, ESP8266_Fram_Record_Struct.Data_RX_BUF, RX_BUF_MAX_LEN);
        // Parse JSON
        ESP8266_Json_Parse(temp_buf);
        // Reset buffer
        ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
        memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
        // Clear flag
        g_MQTT_Data_Ready = 0;
      }
  }
}

void vTask4(void *argument)
{
  static uint8_t buffer_index = 0;
  HAL_StatusTypeDef dma_status;
  size_t pictureLength = 0;

  while(1)
  {
    // Get available buffer
    if (xSemaphoreTake(xBufferSemaphore, portMAX_DELAY) != pdPASS)
    {
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }

    // Reset buffer
    memset((void *)JpegBuffers[buffer_index], 0, sizeof(uint32_t)*BUFFER_SIZE);

    // Start DCMI DMA transfer
    __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
    dma_status = HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT,
                                   (uint32_t)JpegBuffers[buffer_index], BUFFER_SIZE);

    if (dma_status != HAL_OK)
    {
      xSemaphoreGive(xBufferSemaphore);
      continue;
    }

    // Wait for transfer complete semaphore
    if (osSemaphoreAcquire(sem_GetPhoto, osWaitForever) != osOK)
    {
      HAL_DCMI_Stop(&hdcmi);
      xSemaphoreGive(xBufferSemaphore);
      continue;
    }

    // Stop DCMI transfer
    HAL_DCMI_Suspend(&hdcmi);
    HAL_DCMI_Stop(&hdcmi);

    // Calculate actual image size
    pictureLength = BUFFER_SIZE;
    while(pictureLength > 0 && JpegBuffers[buffer_index][pictureLength-1] == 0)
      pictureLength--;
    pictureLength *= sizeof(uint32_t);

    // Ensure we don't exceed buffer size
    if (pictureLength > MAX_IMAGE_SIZE)
    {
      pictureLength = MAX_IMAGE_SIZE;
    }

    // Zero out excess data
    if (pictureLength < MAX_IMAGE_SIZE)
    {
      memset(&JpegBuffers[buffer_index][pictureLength/sizeof(uint32_t)], 0,
             MAX_IMAGE_SIZE - pictureLength);
    }

    // Prepare queue item
    xImageItem.buffer_index = buffer_index;
    xImageItem.length = pictureLength;

    // Send buffer index to queue
    if (xQueueSend(xImageQueue, &xImageItem, 0) != pdPASS)
    {
      xSemaphoreGive(xBufferSemaphore); // Release buffer if send failed
    }
    else
    {
      buffer_index = (buffer_index + 1) % BUFFER_COUNT; // Only update on success
    }

    vTaskDelay(pdMS_TO_TICKS(30)); // Control frame rate
  }
}
/* USER CODE END Application */

