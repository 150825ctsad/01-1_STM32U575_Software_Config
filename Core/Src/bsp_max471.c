#include "adc.h"
#include "stdio.h"

uint16_t Ele;//模拟的值
float EleValue;//进行缩放，最大3A;
 
 
void Electric_start()//放在循环外需要配置循环读取
{
	//HAL_ADCEx_Calibration_Start(&hadc1);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1,HAL_MAX_DELAY);
}

void Electric_GetValue()
{
	Electric_start();
	Ele=HAL_ADC_GetValue(&hadc1);
	EleValue=(Ele/4095.0)*3.0;
	printf("EleValue: %.2fA\r\n",EleValue);

}