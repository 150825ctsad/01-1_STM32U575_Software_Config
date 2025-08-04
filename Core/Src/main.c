/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "main.h"
#include "gpdma.h"
#include "i2c.h"
#include "icache.h"
#include "octospi.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_esp8266.h"
#include "bsp_sht20.h"
#include "bsp_ili9341_4line.h"
#include <string.h>

#include "lvgl.h"
#include "lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "../gui_guider.h"
#include "../events_init.h"

lv_ui guider_ui;
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

//JSON格式
#define JSON_State "{LED1:%d\\Temp\\:%f\\Hum\\:%f}"
#define JSON_Test "{LED1:%d\\Temp\\:%f\\Hum\\:%f}"
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t gRX_BufF[1];
extern struct STRUCT_USART_Fram ESP8266_Fram_Record_Struct;
extern volatile SHT20_TemRH_Val gTemRH_Val;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
__ASM (".global __use_no_semihosting");	//AC6������
 	//��׼����Ҫ��֧�ֺ���
	struct FILE 
 	{
 	  int handle; 
 	};
 	FILE __stdout;
 	//����_sys_exit()�Ա���ʹ�ð�����ģʽ  
	void _sys_exit(int x) 
	{ 
	  x = x; 
	}
	void _ttywrch(int ch)
	{
	  ch = ch;
	}
	//printfʵ���ض���
	int fputc(int ch, FILE *f)	
	{
		uint8_t temp[1] = {ch};
		HAL_UART_Transmit(&huart1, temp, 1, 2);
		return ch;
	}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_GPDMA1_Init();
  MX_ICACHE_Init();
  MX_UART5_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_OCTOSPI1_Init();
  /* USER CODE BEGIN 2 */
  lv_init(); /* lvgl 系统初始化 */
	lv_port_disp_init();
	lv_port_indev_init();
	
	setup_ui(&guider_ui);
	events_init(&guider_ui);
  
	ESP8266_Init(&huart5,(uint8_t*)gRX_BufF,115200);
  ESP8266_STA_MQTTClient();
  HAL_Delay(1000);
  ESP8266_MQTTSUB(User_ESP8266_MQTTServer_Topic);	//订阅Get_State主题(等下修改)
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
{  
    char str[100];
    BSP_SHT20_GetData();    //获取温湿度数据
    sprintf(str,JSON_State, 1,gTemRH_Val.Tem,gTemRH_Val.Hum);
    ILI9341_SetCursor(0, 0); //设置光标位置

    //sprintf(str,JSON_Test, 1,gTemRH_Val.Tem,gTemRH_Val.Hum);    //测试JSON格式
    //ESP8266_MQTTPUB(User_ESP8266_MQTTServer_Topic, str);(需添加发布主题的条件，避免重复发布)
    HAL_Delay(1000);
    
    if (ESP8266_Fram_Record_Struct.InfBit.FramFinishFlag)
    {
        // 增加缓冲区长度检查
        if (ESP8266_Fram_Record_Struct.InfBit.FramLength >= RX_BUF_MAX_LEN) {
            HAL_UART_Transmit(&huart1, (uint8_t*)"接收缓冲区溢出\r\n", 16, 100);
        } else {
            if (strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF, "+MQTTSUBRECV") != NULL)
            {
                ESP8266_Json_Parse(ESP8266_Fram_Record_Struct.Data_RX_BUF);
            }
            else
            {
                char debug_msg[128];
                snprintf(debug_msg, sizeof(debug_msg), "帧头不匹配: %s\r\n", ESP8266_Fram_Record_Struct.Data_RX_BUF);
                HAL_UART_Transmit(&huart1, (uint8_t*)debug_msg, strlen(debug_msg), 100);
            }
        }
        
        // 重置接收缓冲区
        ESP8266_Fram_Record_Struct.InfBit.FramFinishFlag = 0;
        ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
        memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
    }
    HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Disable the internal Pull-Up in Dead Battery pins of UCPD peripheral
   */
  HAL_PWREx_DisableUCPDDeadBattery();
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART5) 
	{
		if(ESP8266_Fram_Record_Struct .InfBit .FramLength < ( RX_BUF_MAX_LEN - 1 ) )
		{
			ESP8266_Fram_Record_Struct .Data_RX_BUF[ ESP8266_Fram_Record_Struct .InfBit .FramLength ++ ]  = gRX_BufF[0];  
			HAL_UART_Receive_IT(&huart5,(uint8_t *)&gRX_BufF, 1);
		}  
	}
}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == UART5)
	{
		ESP8266_Fram_Record_Struct .InfBit .FramFinishFlag = 1;
		HAL_UART_Receive_IT(&huart5,(uint8_t *)&gRX_BufF, 1);
	} 	
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
