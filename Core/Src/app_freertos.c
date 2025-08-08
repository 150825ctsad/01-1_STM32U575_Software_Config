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
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
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
  .stack_size = 128 * 4
};
osThreadId_t Task2Handle;
const osThreadAttr_t Task2_attributes = {
  .name = "Task2",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 256 * 8
};
osThreadId_t Task3Handle;
const osThreadAttr_t Task3_attributes = {
  .name = "Task3",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 256 * 16
};

void vTask1(void *argument);
void vTask2(void *argument);
void vTask3(void *argument);

/* USER CODE END FunctionPrototypes */

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
  const char * pcTaskName = "Task 1 is running\r\n";
  for( ; ; )
  {
      /* 打印出这个任务的名称。 */
      vPrintString( pcTaskName );
      /* 延时一会 */
      vTaskDelay(pdMS_TO_TICKS(300));
			/* 翻转输出绿色LED */
			HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port,GREEN_LED_Pin);
      BSP_SHT20_GetData();
  }
}
void vTask2(void *argument)
{
  const char * pcTaskName = "Task 2 is running\r\n";
  for( ; ; )
  {
      /* 打印出这个任务的名称。 */
      vPrintString( pcTaskName );
      /* 延时一会 */
      vTaskDelay(pdMS_TO_TICKS(100));
      //LCD 刷新
      lv_task_handler();
  }
}
void vTask3(void *argument)
{
  const char * pcTaskName = "Task 3 is running\r\n";
  for( ; ; )
  {
      /* 打印出这个任务的名称。 */
      vPrintString( pcTaskName );
      //ESP8266 处理
      vTaskDelay(pdMS_TO_TICKS(100));
      printf("g_MQTT_Data_Ready:%d\n",g_MQTT_Data_Ready);
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
/* USER CODE END Application */

