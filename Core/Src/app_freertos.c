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
#include "usart.h"
#include "queue.h"

#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "../gui_guider.h"
#include "../events_init.h"
#include "custom.h"
#include "base64.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
osThreadId_t ov2640TaskHandle;
osSemaphoreId_t sem_TakePhoto;
osSemaphoreId_t sem_GetPhoto;

osSemaphoreId_t mqttDataSemaphoreHandle;
const osSemaphoreAttr_t mqttDataSemaphore_attributes = {
  .name = "mqttDataSemaphore"
};
osSemaphoreId_t uart5TxSemaphoreHandle;  // UART5发送信号量
const osSemaphoreAttr_t uart5TxSemaphore_attributes = {
  .name = "uart5TxSemaphore"
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
extern struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct;

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
void vTask1(void *argument);
void vTask2(void *argument);
void vTask3(void *argument);
void vTask4(void *argument);

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
  .stack_size = 1024 * 1
};
osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 4
};
osThreadId_t Task3Handle;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 1024 * 2
};
//osPriorityHigh
osThreadId_t Task4Handle;
const osThreadAttr_t Task4_attributes = {
    .name = "Task4",
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
  sem_TakePhoto = osSemaphoreNew(1, 0, NULL);
  sem_GetPhoto = osSemaphoreNew(1, 0, NULL);
  mqttDataSemaphoreHandle = osSemaphoreNew(1, 0, &mqttDataSemaphore_attributes);
  uart5TxSemaphoreHandle = osSemaphoreNew(1, 0, &uart5TxSemaphore_attributes);  // 新增发送信号量初始化
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
  //Task2Handle = osThreadNew(vTask2, NULL, &Task2_attributes);
  Task3Handle = osThreadNew(vTask3, NULL, &Task3_attributes);
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
    printf("vTask1");

      vPrintString("");
      /* 延时一会 */
      vTaskDelay(pdMS_TO_TICKS(300));
      //BSP_SHT20_GetData();
  }
}
void vTask2(void *argument)
{
  for( ; ; )
  {
    printf("vTask2");

    //LCD 刷新
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void vTask3(void *argument) {
  for( ; ; ) {
        printf("vTask3");     
    // 无数据时永久阻塞，释放CPU给其他任务
    if(osSemaphoreAcquire(mqttDataSemaphoreHandle, osWaitForever) == osOK) {
        // 处理接收数据
        char temp_buf[RX_BUF_MAX_LEN];
        strncpy(temp_buf, ESP8266_Fram_Record_Struct.Data_RX_BUF, RX_BUF_MAX_LEN);
        ESP8266_Json_Parse(temp_buf);
        ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
        memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
    }
    if(osSemaphoreAcquire(uart5TxSemaphoreHandle, osWaitForever) == osOK) {
        // 发送数据
        //HAL_UART_Transmit(&huart5, (uint8_t *)ESP8266_Fram_Record_Struct.Data_RX_BUF, ESP8266_Fram_Record_Struct.InfBit.FramLength, 1000);
    }
  }
}

#define pictureBufferLength 1024*10
static uint32_t JpegBuffer[pictureBufferLength];

void vTask4(void *argument)
{
  for(;;)
  {
    printf("vTask4");

    __HAL_DCMI_ENABLE_IT(&hdcmi,DCMI_IT_FRAME);
    memset((void *)JpegBuffer,0,sizeof(JpegBuffer));
    HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT,(uint32_t)JpegBuffer, pictureBufferLength);
      if(osSemaphoreAcquire(sem_GetPhoto , osWaitForever) == osOK)
    {
      HAL_DCMI_Suspend(&hdcmi);
      HAL_DCMI_Stop(&hdcmi);
      int pictureLength =pictureBufferLength;
				while(pictureLength > 0)//循环计算出接收的JPEG的大小
				{
					if(JpegBuffer[pictureLength-1] != 0x00000000)
					{
          //  printf("pictureLength:%d\n\n",pictureLength);
          //  for(int i = 0;i < pictureLength;i ++)
          //  printf("%08x",JpegBuffer[i]);
          //  printf("\n\n\n");
						break;
					}
					pictureLength--;
				}
				pictureLength*=4;//buf是uint32_t，下面发送是uint8_t,所以长度要*4
        HAL_UART_Transmit(&huart3,(uint8_t*)JpegBuffer,pictureLength,0XFFFFF);
    osDelay(30);
  }
}
}
/* USER CODE END Application */

