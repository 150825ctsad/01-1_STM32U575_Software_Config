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
#include "main.h"
#include <stdio.h>
#include "usart.h"

#include "bsp_sht20.h"
#include "bsp_esp8266.h"
#include "bsp_ov7670.h"
#include "bsp_ili9341_4line.h"

#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "../gui_guider.h"
#include "../events_init.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern volatile uint8_t g_MQTT_Data_Ready; // MQTT数据就绪标志
extern struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct;

extern osThreadId_t cameraTaskHandle;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
void vTask1(void *argument);
void vTask2(void *argument);
void vTask3(void *argument);
void vCameraCaptureTask(void *argument);

void vPrintString( const char *pcString )
{
	//进入临近段保护
	taskENTER_CRITICAL();
	{
		printf( "%s", pcString );
	}
	//退出临近段保护
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
  .stack_size = 1024 * 4
};
osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 8
};
osThreadId_t Task3Handle;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
osThreadId_t cameraTaskHandle;
const osThreadAttr_t cameraTask_attributes = {
    .name = "cameraTask",
    .priority = (osPriority_t) osPriorityLow,  
    .stack_size = 1024 * 16  
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
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
    Task1Handle = osThreadNew(vTask1, NULL, &Task1_attributes);
    Task2Handle = osThreadNew(vTask2, NULL, &Task2_attributes);
    Task3Handle = osThreadNew(vTask3, NULL, &Task3_attributes);
    cameraTaskHandle = osThreadNew(vCameraCaptureTask, NULL, &cameraTask_attributes);

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
      /* 延时一会 */
      vTaskDelay(pdMS_TO_TICKS(300));
      //BSP_SHT20_GetData();
  }
}
void vTask2(void *argument)
{
  uint16_t *frame_buffer = NULL;
  for( ; ; )
  {
      // 添加队列接收逻辑
      if (g_capturing && xQueueReceive(xFrameQueue, &frame_buffer, pdMS_TO_TICKS(10)) == pdPASS) {
          // 将帧数据显示到LCD
          _HW_FillFrame(0, 0, CAMERA_WIDTH, CAMERA_HEIGHT, (uint16_t*)frame_buffer);
      }
      //LCD 刷新
      lv_task_handler();
      osDelay(1);
  }
}
void vTask3(void *argument)
{
  for( ; ; )
  {
      vPrintString("");
      //ESP8266 处理
      vTaskDelay(pdMS_TO_TICKS(100));
      //printf("g_MQTT_Data_Ready:%d\n",g_MQTT_Data_Ready);
    if (g_MQTT_Data_Ready)
      {
        // 复制数据到局部缓冲区（避免解析过程中被新数据覆盖）
        char temp_buf[RX_BUF_MAX_LEN];
        strncpy(temp_buf, ESP8266_Fram_Record_Struct.Data_RX_BUF, RX_BUF_MAX_LEN);
        // 解析JSON
        ESP8266_Json_Parse(temp_buf);
        // 重置缓冲区（
        ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
        memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
        // 清除标志位
        g_MQTT_Data_Ready = 0;
      }
  }
}
void vCameraCaptureTask(void *argument) {

    #define LINE_BYTES     (CAMERA_WIDTH * 2)  // 每行字节数（RGB565格式）

    uint32_t line_index = 0;  // 当前行索引（0 ~ CAMERA_HEIGHT-1）
    uint8_t *current_buffer = g_image_buffer1;  // 当前缓冲区指针

    for (;;) {
        // 仅在采集标志激活时处理（由LVGL按钮控制）
        if (g_capturing) {
          //printf("vCameraCaptureTask \n");
            if (xSemaphoreTake(xImageSemaphore, pdMS_TO_TICKS(100)) == pdTRUE) {
                // 计算当前行在缓冲区中的偏移量
                uint32_t buffer_offset = line_index * LINE_BYTES;
                // 读取一行FIFO数据到当前缓冲区（RGB565格式，2字节/像素）
                FIFO_ReadData(&current_buffer[buffer_offset], LINE_BYTES);
                // 行索引递增，判断是否完成一帧
                line_index++;
                //printf("line_index:%d\n",line_index);
                if (line_index >= CAMERA_HEIGHT) {
                    // 一帧完成：切换缓冲区并触发帧处理回调
                    printf("ok");

                    OV7670_CaptureDoneCallback();
                    // 重置行索引，切换双缓冲区
                    line_index = 0;
                    current_buffer = (g_current_buffer_idx == 0) ? g_image_buffer1 : g_image_buffer2;
                }
            } else {
                // 信号量超时（可能是采集停止或错误）
                if (!g_capturing) {
                    line_index = 0;  // 重置行索引
                    current_buffer = g_image_buffer1;  // 重置缓冲区
                }
            }
        } //else {
            // 采集未激活时挂起任务，降低CPU占用
          //  vTaskSuspend(NULL);
        //}
    }
}
/* USER CODE END Application */

