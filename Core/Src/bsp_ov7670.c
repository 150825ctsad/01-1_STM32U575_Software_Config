#include "bsp_ov7670.h"
#include "bsp_ili9341_4line.h"
#include "spi.h"
#include "i2c.h"
#include "main.h"
#include "gpdma.h"

// 使用I2C向OV7670发送配置指令
HAL_StatusTypeDef OV7670_WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t data[2] = {reg, value};
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, OV7670_DEVICE_WRITE_ADDRESS, data, 2, 10);
    return status;
}
 
// 使用I2C读取OV7670的状态寄存器
HAL_StatusTypeDef OV7670_ReadReg(uint8_t reg, uint8_t *value)
{
    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c2, OV7670_DEVICE_READ_ADDRESS, &reg, 1, 10);
    if(status != HAL_OK) return status;
    status = HAL_I2C_Master_Receive(&hi2c2, OV7670_DEVICE_READ_ADDRESS, value, 1, 10);
    return status;
}
volatile uint8_t g_capturing = 0;

void OV7670_Reset(void){
	HAL_GPIO_WritePin(RESTE_GPIO_Port, RESTE_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RESTE_GPIO_Port, RESTE_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
}

// 复位写指针
void FIFO_ResetWPoint(void){
    HAL_GPIO_WritePin(WRST_GPIO_Port, WRST_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(WRST_GPIO_Port, WRST_Pin, GPIO_PIN_SET);
}
// 复位读指针
void FIFO_ResetRPoint(void){
    HAL_GPIO_WritePin(RRST_GPIO_Port, RRST_Pin, GPIO_PIN_RESET);  
    HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_RESET);  
    HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RRST_GPIO_Port, RRST_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_SET);
}
void FIFO_OpenReadData(void){
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_RESET);     // 允许写入
}
void FIFO_CloseReadData(void){
    HAL_GPIO_WritePin(OE_GPIO_Port, OE_Pin, GPIO_PIN_SET);     // 禁止写入
}
void FIFO_ReadData(uint8_t* cache, uint16_t len){
    FIFO_ResetRPoint();
    FIFO_OpenReadData();

    // 确保DMA通道空闲
    if (HAL_DMA_GetState(&handle_GPDMA1_Channel12) == HAL_DMA_STATE_BUSY) {
        HAL_DMA_Abort(&handle_GPDMA1_Channel12);
    }

    // 启动DMA传输 (GPIOC->IDR到缓存区)
    if (HAL_DMA_Start(&handle_GPDMA1_Channel12, (uint32_t)&GPIOC->IDR, (uint32_t)cache, len) != HAL_OK) {
        Error_Handler();
    }

    // 等待DMA传输完成
    HAL_DMA_PollForTransfer(&handle_GPDMA1_Channel12, HAL_DMA_FULL_TRANSFER, HAL_MAX_DELAY);

//  	 for(uint16_t index = 0; index < len; ++index){
//   	     HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_RESET);
//   	     cache[index] = 
//		(HAL_GPIO_ReadPin(D0_GPIO_Port, D0_Pin)) |
//		(HAL_GPIO_ReadPin(D1_GPIO_Port, D1_Pin) << 1) |
//		(HAL_GPIO_ReadPin(D2_GPIO_Port, D2_Pin) << 2) |
//		(HAL_GPIO_ReadPin(D3_GPIO_Port, D3_Pin) << 3) |
//		(HAL_GPIO_ReadPin(D4_GPIO_Port, D4_Pin) << 4) |
//		(HAL_GPIO_ReadPin(D5_GPIO_Port, D5_Pin) << 5) |
//		(HAL_GPIO_ReadPin(D6_GPIO_Port, D6_Pin) << 6) |
//		(HAL_GPIO_ReadPin(D7_GPIO_Port, D7_Pin) << 7);
//        HAL_GPIO_WritePin(RCK_GPIO_Port, RCK_Pin, GPIO_PIN_SET);
//    }
    FIFO_CloseReadData();
}

void OV7670_Init(void) {

    OV7670_WriteReg(0x3a, 0x04);
	OV7670_WriteReg(0x40, 0xd0);
	OV7670_WriteReg(0x12, 0x14);
	OV7670_WriteReg(0x32, 0x80);
	OV7670_WriteReg(0x17, 0x16);
	OV7670_WriteReg(0x18, 0x04);
	OV7670_WriteReg(0x19, 0x02);
	OV7670_WriteReg(0x1a, 0x7b);
	OV7670_WriteReg(0x03, 0x06);
	OV7670_WriteReg(0x0c, 0x00);
	OV7670_WriteReg(0x3e, 0x00);
	OV7670_WriteReg(0x70, 0x3a);
	OV7670_WriteReg(0x71, 0x35);
	OV7670_WriteReg(0x72, 0x11);
	OV7670_WriteReg(0x73, 0x00);
	OV7670_WriteReg(0xa2, 0x02);
	OV7670_WriteReg(0x11, 0x81);
	
	OV7670_WriteReg(0x7a, 0x20);
	OV7670_WriteReg(0x7b, 0x1c);
	OV7670_WriteReg(0x7c, 0x28);
	OV7670_WriteReg(0x7d, 0x3c);
	OV7670_WriteReg(0x7e, 0x55);
	OV7670_WriteReg(0x7f, 0x68);
	OV7670_WriteReg(0x80, 0x76);
	OV7670_WriteReg(0x81, 0x80);
	OV7670_WriteReg(0x82, 0x88);
	OV7670_WriteReg(0x83, 0x8f);
	OV7670_WriteReg(0x84, 0x96);
	OV7670_WriteReg(0x85, 0xa3);
	OV7670_WriteReg(0x86, 0xaf);
	OV7670_WriteReg(0x87, 0xc4);
	OV7670_WriteReg(0x88, 0xd7);
	OV7670_WriteReg(0x89, 0xe8);
	
	OV7670_WriteReg(0x13, 0xe0);
	OV7670_WriteReg(0x00, 0x00);
	
	OV7670_WriteReg(0x10, 0x00);
	OV7670_WriteReg(0x0d, 0x00);
	OV7670_WriteReg(0x14, 0x28);
	OV7670_WriteReg(0xa5, 0x05);
	OV7670_WriteReg(0xab, 0x07);
	OV7670_WriteReg(0x24, 0x75);
	OV7670_WriteReg(0x25, 0x63);
	OV7670_WriteReg(0x26, 0xA5);
	OV7670_WriteReg(0x9f, 0x78);
	OV7670_WriteReg(0xa0, 0x68);
	OV7670_WriteReg(0xa1, 0x03);
	OV7670_WriteReg(0xa6, 0xdf);
	OV7670_WriteReg(0xa7, 0xdf);
	OV7670_WriteReg(0xa8, 0xf0);
	OV7670_WriteReg(0xa9, 0x90);
	OV7670_WriteReg(0xaa, 0x94);
	OV7670_WriteReg(0x13, 0xe5);

	OV7670_WriteReg(0x0e, 0x61);
	OV7670_WriteReg(0x0f, 0x4b);
	OV7670_WriteReg(0x16, 0x02);
	OV7670_WriteReg(0x1e, 0x37);
	OV7670_WriteReg(0x21, 0x02);
	OV7670_WriteReg(0x22, 0x91);
	OV7670_WriteReg(0x29, 0x07);
	OV7670_WriteReg(0x33, 0x0b);
	OV7670_WriteReg(0x35, 0x0b);
	OV7670_WriteReg(0x37, 0x1d);
	OV7670_WriteReg(0x38, 0x71);
	OV7670_WriteReg(0x39, 0x2a);
	OV7670_WriteReg(0x3c, 0x78);
	OV7670_WriteReg(0x4d, 0x40);
	OV7670_WriteReg(0x4e, 0x20);
	OV7670_WriteReg(0x69, 0x00);
	OV7670_WriteReg(0x6b, 0x60);
	OV7670_WriteReg(0x74, 0x19);
	OV7670_WriteReg(0x8d, 0x4f);
	OV7670_WriteReg(0x8e, 0x00);
	OV7670_WriteReg(0x8f, 0x00);
	OV7670_WriteReg(0x90, 0x00);
	OV7670_WriteReg(0x91, 0x00);
	OV7670_WriteReg(0x92, 0x00);
	OV7670_WriteReg(0x96, 0x00);
	OV7670_WriteReg(0x9a, 0x80);
	OV7670_WriteReg(0xb0, 0x84);
	OV7670_WriteReg(0xb1, 0x0c);
	OV7670_WriteReg(0xb2, 0x0e);
	OV7670_WriteReg(0xb3, 0x82);
	OV7670_WriteReg(0xb8, 0x0a);



	OV7670_WriteReg(0x43, 0x14);
	OV7670_WriteReg(0x44, 0xf0);
	OV7670_WriteReg(0x45, 0x34);
	OV7670_WriteReg(0x46, 0x58);
	OV7670_WriteReg(0x47, 0x28);
	OV7670_WriteReg(0x48, 0x3a);
	OV7670_WriteReg(0x59, 0x88);
	OV7670_WriteReg(0x5a, 0x88);
	OV7670_WriteReg(0x5b, 0x44);
	OV7670_WriteReg(0x5c, 0x67);
	OV7670_WriteReg(0x5d, 0x49);
	OV7670_WriteReg(0x5e, 0x0e);
	OV7670_WriteReg(0x64, 0x04);
	OV7670_WriteReg(0x65, 0x20);
	OV7670_WriteReg(0x66, 0x05);
	OV7670_WriteReg(0x94, 0x04);
	OV7670_WriteReg(0x95, 0x08);
	OV7670_WriteReg(0x6c, 0x0a);
	OV7670_WriteReg(0x6d, 0x55);
	OV7670_WriteReg(0x6e, 0x11);
	OV7670_WriteReg(0x6f, 0x9f);
	OV7670_WriteReg(0x6a, 0x40);
	OV7670_WriteReg(0x01, 0x40);
	OV7670_WriteReg(0x02, 0x40);
	OV7670_WriteReg(0x13, 0xe7);
	OV7670_WriteReg(0x15, 0x00);  
	
	
	OV7670_WriteReg(0x4f, 0x80);
	OV7670_WriteReg(0x50, 0x80);
	OV7670_WriteReg(0x51, 0x00);
	OV7670_WriteReg(0x52, 0x22);
	OV7670_WriteReg(0x53, 0x5e);
	OV7670_WriteReg(0x54, 0x80);
	OV7670_WriteReg(0x58, 0x9e);
	
	OV7670_WriteReg(0x41, 0x08);
	OV7670_WriteReg(0x3f, 0x00);
	OV7670_WriteReg(0x75, 0x05);
	OV7670_WriteReg(0x76, 0xe1);
	OV7670_WriteReg(0x4c, 0x00);
	OV7670_WriteReg(0x77, 0x01);
	OV7670_WriteReg(0x3d, 0xc2);	
	OV7670_WriteReg(0x4b, 0x09);
	OV7670_WriteReg(0xc9, 0x60);
	OV7670_WriteReg(0x41, 0x38);
	OV7670_WriteReg(0x56, 0x40);
	
	OV7670_WriteReg(0x34, 0x11);
	OV7670_WriteReg(0x3b, 0x02); 
								
	OV7670_WriteReg(0xa4, 0x89);
	OV7670_WriteReg(0x96, 0x00);
	OV7670_WriteReg(0x97, 0x30);
	OV7670_WriteReg(0x98, 0x20);
	OV7670_WriteReg(0x99, 0x30);
	OV7670_WriteReg(0x9a, 0x84);
	OV7670_WriteReg(0x9b, 0x29);
	OV7670_WriteReg(0x9c, 0x03);
	OV7670_WriteReg(0x9d, 0x4c);
	OV7670_WriteReg(0x9e, 0x3f);
	OV7670_WriteReg(0x78, 0x04);
	
	OV7670_WriteReg(0x79, 0x01);
	OV7670_WriteReg(0xc8, 0xf0);
	OV7670_WriteReg(0x79, 0x0f);
	OV7670_WriteReg(0xc8, 0x00);
	OV7670_WriteReg(0x79, 0x10);
	OV7670_WriteReg(0xc8, 0x7e);
	OV7670_WriteReg(0x79, 0x0a);
	OV7670_WriteReg(0xc8, 0x80);
	OV7670_WriteReg(0x79, 0x0b);
	OV7670_WriteReg(0xc8, 0x01);
	OV7670_WriteReg(0x79, 0x0c);
	OV7670_WriteReg(0xc8, 0x0f);
	OV7670_WriteReg(0x79, 0x0d);
	OV7670_WriteReg(0xc8, 0x20);
	OV7670_WriteReg(0x79, 0x09);
	OV7670_WriteReg(0xc8, 0x80);
	OV7670_WriteReg(0x79, 0x02);
	OV7670_WriteReg(0xc8, 0xc0);
	OV7670_WriteReg(0x79, 0x03);
	OV7670_WriteReg(0xc8, 0x40);
	OV7670_WriteReg(0x79, 0x05);
	OV7670_WriteReg(0xc8, 0x30);
	OV7670_WriteReg(0x79, 0x26); 
	OV7670_WriteReg(0x09, 0x00);	
                
}
