#include "bsp_max471.h"
#include "adc.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"

uint16_t adc_dma_buffer[1] = {0};  // 存储ADC转换结果
float adc_voltage = 0.0f;          // 存储转换后的电压值


float ADC_GetVoltage(void)
{
  HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adc_dma_buffer,1);

  adc_voltage = (adc_dma_buffer[0] * 3.3f) / (1 << 14);
  printf("adc_voltage = %f\n", adc_voltage);

  return adc_voltage;
}