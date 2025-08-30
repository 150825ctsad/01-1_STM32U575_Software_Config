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
#include <stdlib.h>
#include <stdio.h>
#include "usart.h"
#include "queue.h"
#include "cJSON.h"

#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "../gui_guider.h"
#include "../events_init.h"
#include "custom.h"
#include "base64.h"
#include "tjpgd.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
// 摄像头数据传输结构体
typedef struct {
  uint8_t* data;       // 数据指针
  uint32_t length;     // 数据长度
} CameraData_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//信号
osSemaphoreId_t sem_TakePhoto;
osSemaphoreId_t sem_GetPhoto;
osSemaphoreId_t sem_PhotoTrigger;

osSemaphoreId_t mqttDataSemaphoreHandle;
osSemaphoreId_t base64SemaphoreHandle;
// 消息队列句柄
osMessageQueueId_t cameraQueueHandle;
//锁
osMutexId_t jpegBufferMutex; 
osMutexId_t bufferMutex;
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
void vTask5(void *argument);

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
  .priority = (osPriority_t) osPriorityLow5,
  .stack_size = 1024 * 4
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
  .priority = (osPriority_t) osPriorityHigh,
  .stack_size = 1024 * 4
};
//osPriorityHigh
osThreadId_t Task4Handle;
const osThreadAttr_t Task4_attributes = {
    .name = "Task4",
    .priority = (osPriority_t) osPriorityNormal5,  
    .stack_size = 1024 * 16 
};
osThreadId_t Task5Handle;
const osThreadAttr_t Task5_attributes = {
    .name = "Task5",
    .priority = (osPriority_t) osPriorityHigh7,  
    .stack_size = 1024 * 1 
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
  jpegBufferMutex = osMutexNew(NULL); 
  bufferMutex = osMutexNew(NULL);
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  sem_TakePhoto = osSemaphoreNew(1, 0, NULL);
  sem_GetPhoto = osSemaphoreNew(1, 0, NULL);
  sem_PhotoTrigger = osSemaphoreNew(1, 0, NULL);

  mqttDataSemaphoreHandle = osSemaphoreNew(1, 0, NULL);
  base64SemaphoreHandle = osSemaphoreNew(1, 0, NULL);
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  // 队列长度为5，每个消息大小为结构体大小
cameraQueueHandle = osMessageQueueNew(5, sizeof(CameraData_t), NULL);
if (cameraQueueHandle == NULL) {
  printf("Failed to create camera message queue\n");
}
  /* USER CODE END RTOS_QUEUES */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  Task1Handle = osThreadNew(vTask1, NULL, &Task1_attributes);
  Task2Handle = osThreadNew(vTask2, NULL, &Task2_attributes);
  Task3Handle = osThreadNew(vTask3, NULL, &Task3_attributes);
  Task4Handle = osThreadNew(vTask4, NULL, &Task4_attributes);
  Task5Handle = osThreadNew(vTask5, NULL, &Task5_attributes);

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
//JSON格式
#define JSON_Sensor_Data "{\\\"Temp\\\":%.2f\\\,\\\"Hum\\\":%.2f\\\,\\\"Ele\\\":%.2f\\\,\\\"Light\\\":%.2f\\\,\\\"Relay\\\":%s\\\,\\\"BEEP\\\":%s\\\,\\\"LED\\\":%s}"
//#define JSON_State       "{\\\"Relay\\\":%s\\\,\\\"BEEP\\\":%s\\\,\\\"LED\\\":%s}"

void vTask1(void *argument)
{
  for( ; ; )
  {
    BSP_SHT20_GetData();    //获取温湿度数据
    Electric_GetValue();

    GPIO_PinState relay_state = HAL_GPIO_ReadPin(GPIOA, RELAY_Pin);       //GPIOA7 - 继电器
    GPIO_PinState led_state = HAL_GPIO_ReadPin(GPIOC, BLUE_LED_Pin);     //PC13 - LED
    GPIO_PinState beep_state = HAL_GPIO_ReadPin(GPIOA, RUN_BEEP_Pin);    //PA15 - 蜂鸣器
    char* relay_str = (relay_state == GPIO_PIN_RESET) ? "on" : "off";
    char* led_str = (led_state == GPIO_PIN_RESET) ? "on" : "off";
    char* beep_str = (beep_state == GPIO_PIN_RESET) ? "on" : "off";

    float LightValue = 1.23; 

    char str1[1024-64];
    sprintf(str1,JSON_Sensor_Data,gTemRH_Val.Tem,gTemRH_Val.Hum,EleValue,LightValue,relay_str,beep_str,led_str);
    ESP8266_MQTTPUB(User_ESP8266_MQTTServer_Topic, str1);
    //char str2[1024-64];
    //sprintf(str2,JSON_State,"no","no","no");
    //ESP8266_MQTTPUB(User_ESP8266_MQTTServer_Topic, str2);

    vTaskDelay(1000);
  }
}

void vTask2(void *argument)
{
  for( ; ; )
  {
    printf("vTask2");

    // LCD 刷新
    lv_task_handler();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void vTask3(void *argument) {
  for( ; ; ) {
      //printf("vTask3");   
    // 无数据时永久阻塞，释放CPU给其他任务
    if(osSemaphoreAcquire(mqttDataSemaphoreHandle, osWaitForever) == osOK) {
        
        char temp_buf[RX_BUF_MAX_LEN];
        strncpy(temp_buf, ESP8266_Fram_Record_Struct.Data_RX_BUF, RX_BUF_MAX_LEN);  
        // 获取缓冲区互斥锁
        osMutexAcquire(bufferMutex, osWaitForever);
        
        // 处理接收数据
        ESP8266_Json_Parse(temp_buf);  
        ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
        memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
        
        // 释放缓冲区互斥锁
        osMutexRelease(bufferMutex);
    }
  }
}

#define pictureBufferLength 1024*2 //2*4kb //10*4kb
static uint32_t JpegBuffer[pictureBufferLength];

int byteLength;

void vTask4(void *argument) {
    int pictureLength = pictureBufferLength;
    CameraData_t cameraData;
    for(;;) {
      //printf("vTask4");
      
      //vTaskDelay(pdMS_TO_TICKS(500));
        // 启用DCMI帧中断并初始化缓冲区
      __HAL_DCMI_ENABLE_IT(&hdcmi, DCMI_IT_FRAME);
      memset((void *)JpegBuffer, 0, sizeof(JpegBuffer));
      HAL_DCMI_Start_DMA(&hdcmi, DCMI_MODE_SNAPSHOT, (uint32_t)JpegBuffer, pictureBufferLength);

    if(osSemaphoreAcquire(sem_GetPhoto , osWaitForever) == osOK)
    {
      HAL_DCMI_Suspend(&hdcmi);
      HAL_DCMI_Stop(&hdcmi);
      int pictureLength =pictureBufferLength;
				while(pictureLength > 0)//循环计算出接收的JPEG的大小
				{
					if(JpegBuffer[pictureLength-1] != 0x00000000)
					{
            //printf("pictureLength:%d\n\n",pictureLength);
            //for(int i = 0;i < pictureLength;i ++)
            //printf("%08X",JpegBuffer[i]);
            //printf("\n\n\n");
						break;
					}
					pictureLength--;
				}               
        // 准备消息数据
        cameraData.data = (uint8_t*)JpegBuffer;
        cameraData.length = pictureLength * 4;
        
        // 发送数据到消息队列
        osStatus_t status = osMessageQueuePut(cameraQueueHandle, &cameraData, 0, 100);
        if (status != osOK) {
          printf("Failed to send data to queue\n");
        }
        osDelay(500);
      }
  }
}
static char base64_encoded[(pictureBufferLength * 4) * 4 / 3 + 1024]; // Increased padding

void vTask5(void *argument)
{
  CameraData_t receivedData;
  for( ; ; )
  {
    //printf("vTask5");
    
    // 等待JPEG数据准备信号量
    if(osSemaphoreAcquire(sem_PhotoTrigger, osWaitForever) == osOK){
        //if (byteLength <= 0) {
        //    printf("vTask4: Invalid JPEG data length\n");
        //    continue;
        //} 

        // 获取互斥锁保护JPEG缓冲区
        osMessageQueueGet(cameraQueueHandle, &receivedData, NULL, osWaitForever);
        osMutexAcquire(jpegBufferMutex, osWaitForever);
        // Base64编码（从vTask4迁移过来）
        size_t output_len = sizeof(base64_encoded);
        const int encode_result = jpeg_to_base64(
            receivedData.data,       // 使用队列接收的数据
            receivedData.length,     // 使用队列接收的长度
            base64_encoded,        // 输出缓冲区
            &output_len            // 输出长度指针
        );
        osMutexRelease(jpegBufferMutex);  // 及时释放互斥锁

        //// 处理编码结果
        //if (encode_result != 0) {
        //    printf("vTask5: Base64 encode failed, error code: %d\n", encode_result);
        //    continue;
        //}
        //if (output_len > sizeof(base64_encoded)) {
        //    printf("vTask5: Base64 buffer overflow (required: %zu, available: %zu)\n", output_len, sizeof(base64_encoded));
        //    continue;
        //}

        // 发布数据（原有逻辑保留）
        char len_str[16];
        sprintf(len_str, "%zu", strlen(base64_encoded));
        ESP8266_MQTTPUBRAW("test", len_str);
        HAL_UART_Transmit(&huart5, (uint8_t*)base64_encoded, strlen(base64_encoded), 0xFFFF);
        memset(base64_encoded, 0, sizeof(base64_encoded));
        
        osDelay(500);
      }
  }
}
/* USER CODE END Application */

