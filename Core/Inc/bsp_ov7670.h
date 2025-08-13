#ifndef __BSP_OV7670_H__
#define __BSP_OV7670_H__

#include "main.h"
#include "i2c.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

#define OV7670_DEVICE_WRITE_ADDRESS    0x42
#define OV7670_DEVICE_READ_ADDRESS     0x43

#define CAMERA_WIDTH    320
#define CAMERA_HEIGHT   240
#define CAMERA_FRAME_SIZE (CAMERA_WIDTH * CAMERA_HEIGHT * 2)  // 153600字节
#define FRAME_QUEUE_LEN 2                  // 队列长度（缓存2帧，避免丢帧）

// 声明消息队列句柄
extern QueueHandle_t xFrameQueue;

// 声明图像缓冲区（使用2个缓冲区乒乓操作，避免采集/显示冲突）
extern uint8_t g_image_buffer1[CAMERA_FRAME_SIZE];
extern uint8_t g_image_buffer2[CAMERA_FRAME_SIZE];
extern volatile uint8_t g_current_buffer_idx;  // 当前使用的缓冲区索引

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

void OV7670_CaptureDoneCallback(void);

#endif /* __BSP_OV7670_H */