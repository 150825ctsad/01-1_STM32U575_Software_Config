/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
 extern volatile uint8_t g_processing_frame;

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BLUE_LED_Pin GPIO_PIN_13
#define BLUE_LED_GPIO_Port GPIOC
#define VSYNC_Pin GPIO_PIN_0
#define VSYNC_GPIO_Port GPIOA
#define VSYNC_EXTI_IRQn EXTI0_IRQn
#define HREF_Pin GPIO_PIN_1
#define HREF_GPIO_Port GPIOA
#define HREF_EXTI_IRQn EXTI1_IRQn
#define LCD_DXC_Pin GPIO_PIN_4
#define LCD_DXC_GPIO_Port GPIOA
#define D0_Pin GPIO_PIN_4
#define D0_GPIO_Port GPIOC
#define D1_Pin GPIO_PIN_5
#define D1_GPIO_Port GPIOC
#define RRST_Pin GPIO_PIN_0
#define RRST_GPIO_Port GPIOB
#define WRST_Pin GPIO_PIN_1
#define WRST_GPIO_Port GPIOB
#define OE_Pin GPIO_PIN_2
#define OE_GPIO_Port GPIOB
#define STROBE_Pin GPIO_PIN_12
#define STROBE_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_6
#define D2_GPIO_Port GPIOC
#define D3_Pin GPIO_PIN_7
#define D3_GPIO_Port GPIOC
#define D4_Pin GPIO_PIN_8
#define D4_GPIO_Port GPIOC
#define D5_Pin GPIO_PIN_9
#define D5_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOA
#define USER_KEY_Pin GPIO_PIN_12
#define USER_KEY_GPIO_Port GPIOA
#define RUN_BEEP_Pin GPIO_PIN_15
#define RUN_BEEP_GPIO_Port GPIOA
#define D6_Pin GPIO_PIN_10
#define D6_GPIO_Port GPIOC
#define D7_Pin GPIO_PIN_11
#define D7_GPIO_Port GPIOC
#define WR_CTR_Pin GPIO_PIN_3
#define WR_CTR_GPIO_Port GPIOB
#define RCK_Pin GPIO_PIN_4
#define RCK_GPIO_Port GPIOB
#define TP_INT_Pin GPIO_PIN_5
#define TP_INT_GPIO_Port GPIOB
#define TP_INT_EXTI_IRQn EXTI5_IRQn
#define PWDN_Pin GPIO_PIN_8
#define PWDN_GPIO_Port GPIOB
#define RESTE_Pin GPIO_PIN_9
#define RESTE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define LCD_RST_Pin GPIO_PIN_8
#define LCD_RST_GPIO_Port GPIOA
#define LCD_DCX_Pin GPIO_PIN_4
#define LCD_DCX_GPIO_Port GPIOA
#define TP_RST_Pin GPIO_PIN_11
#define TP_RST_GPIO_Port GPIOA
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
