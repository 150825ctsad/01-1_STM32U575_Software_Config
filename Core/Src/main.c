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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bsp_esp8266.h"
#include "bsp_sht20.h"
#include "bsp_ili9341_4line.h"
#include "bsp_ft6336.h"
#include "bsp_ospi_w25q128.h"
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

  void OSPI_W25Qxx_mmap(void)		//Flash读写测试
{
	int32_t OSPI_Status ; 		 //检测标志位
	//
	OSPI_Status = OSPI_W25Qxx_MemoryMappedMode(); //配置OSPI为内存映射模式
	if( OSPI_Status == OSPI_W25Qxx_OK )
	{
		printf ("\r\n内存映射模式设置成功>>>>\r\n");		
	}
	else
	{
		printf ("\r\n内存映射模式设置失败>>>>\r\n");
		Error_Handler();
	}	
}
void Update_Backlight(uint8_t pDutyRatio)
{		
	//参数检查
	if((pDutyRatio < 5) || (pDutyRatio > 100)) return;
	//停止PWM信号
 	HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_3);
	//设置占空比值
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,  pDutyRatio*10);
	//设置当前计数值
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	//启动PWM信号
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
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
  MX_TIM16_Init();
  MX_TIM2_Init();
  MX_TIM17_Init();
  MX_OCTOSPI1_Init();
  /* USER CODE BEGIN 2 */
  OSPI_W25Qxx_Init();	//初始化W25Q128
  OSPI_W25Qxx_mmap();

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

  Update_Backlight(80); //设置背光亮度
  HAL_TIM_Base_Start_IT(&htim16);//开启定时器16开启,系统任务调度开始
  HAL_TIM_Base_Start_IT(&htim17);//开启定时器17开启,设备控制任务开始 
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
{  
  lv_task_handler();
  HAL_Delay(5);

    //char str[100];
    //BSP_SHT20_GetData();    //获取温湿度数据
    //sprintf(str,JSON_State, 1,gTemRH_Val.Tem,gTemRH_Val.Hum);
    //ESP8266_MQTTPUB(User_ESP8266_MQTTServer_Topic, str);
/*
  if (ESP8266_Fram_Record_Struct.InfBit.FramFinishFlag)
    {
        // 增加缓冲区长度检查
        if (ESP8266_Fram_Record_Struct.InfBit.FramLength >= RX_BUF_MAX_LEN) {
            HAL_UART_Transmit(&huart1, (uint8_t*)"接收缓冲区溢出\r\n", 16, 100);
        } else if (strstr(ESP8266_Fram_Record_Struct.Data_RX_BUF, "+MQTTSUBRECV") != NULL)
        {
            ESP8266_Json_Parse(ESP8266_Fram_Record_Struct.Data_RX_BUF);
            // 重置接收缓冲区
            ESP8266_Fram_Record_Struct.InfBit.FramFinishFlag = 0;
            ESP8266_Fram_Record_Struct.InfBit.FramLength = 0;
            memset(ESP8266_Fram_Record_Struct.Data_RX_BUF, 0, RX_BUF_MAX_LEN);
        }
    }
*/
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

//定时器16
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	static uint8_t p_Time16Cnt = 0,p_Time17Cnt = 0;
	/***************************************************************************************/
	//定时器16进行1ms任务中断
	if (htim->Instance == htim16.Instance) 
	{
		p_Time16Cnt++;
		//lvgl的 1ms 心跳函数
		lv_tick_inc(1); 
		//1000ms运行一次
		if(!(p_Time16Cnt % 1000))  
		{
			p_Time16Cnt = 0; 
		}
	}
	/***************************************************************************************/
	//定时器17进行100ms任务中断
	if (htim->Instance == htim17.Instance) 
	{
		p_Time17Cnt++;
		if(!(p_Time17Cnt % 100))  //10s进行一次下列代码 
		{
			p_Time17Cnt = 0; 
		}
	}
	/***************************************************************************************/
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);
}

//UART5接收中断回调函数
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

/**
  * @brief  EXTI line rising detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
uint8_t TouchPress = 0; //触摸按下标志
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	//触摸按下事件
	if((!HAL_GPIO_ReadPin(TP_INT_GPIO_Port,TP_INT_Pin)) && (GPIO_Pin == TP_INT_Pin))
	{
		TouchPress = 1;
	}
}
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);
	//触摸释放事件
	if(HAL_GPIO_ReadPin(TP_INT_GPIO_Port,TP_INT_Pin) && TouchPress)
	{
		FT6336_irq_fuc();	//触摸中断产生
		TouchPress = 0;	//清除触摸标志
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
