#ifndef __BSP_OV7670_H__
#define __BSP_OV7670_H__

#include "main.h"
#include "i2c.h"

#include "FreeRTOS.h"
#include "semphr.h"

#define OV7670_DEVICE_WRITE_ADDRESS    0x42
#define OV7670_DEVICE_READ_ADDRESS     0x43

#define CAMERA_WIDTH    320
#define CAMERA_HEIGHT   240
#define CAMERA_FRAME_SIZE (CAMERA_WIDTH * CAMERA_HEIGHT * 2)  // 153600字节

extern uint8_t g_image_buffer[CAMERA_FRAME_SIZE] __attribute__((section(".ccmram")));  // 图像缓冲区
extern volatile uint8_t g_image_ready;             // 一帧数据就绪标志（0：未就绪，1：就绪）
extern SemaphoreHandle_t xImageSemaphore;  

extern volatile uint8_t g_capturing;                      
  
extern SemaphoreHandle_t xImageMutex;

void OV7670_Reset(void);

HAL_StatusTypeDef OV7670_Init(void);

void FIFO_ResetWPoint(void);
void FIFO_ResetRPoint(void);
void FIFO_OpenReadData(void);
void FIFO_CloseReadData(void);
void FIFO_ReadData(uint8_t* cache, uint16_t len);

void OV7670_StartCapture(void);
void OV7670_StopCapture(void);

#endif /* __BSP_OV7670_H */
