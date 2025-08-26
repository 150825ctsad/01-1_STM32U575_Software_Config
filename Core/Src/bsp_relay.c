#include "stm32u5xx_hal.h"

uint16_t Reley_state=0;//�̵���״̬λ
void Relay_ON(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
	Reley_state=1;
}
 
void Relay_OFF(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
   	Reley_state=0;
}
 
void Relay_Toggle(void)
{
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_15);
}
   
 