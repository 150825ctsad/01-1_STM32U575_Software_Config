#ifndef __BSP_OV7670_H__
#define __BSP_OV7670_H__

#include "main.h"
#include "i2c.h"

#define OV7670_DEVICE_WRITE_ADDRESS    0x42
#define OV7670_DEVICE_READ_ADDRESS     0x43

#define OV7670_FRAME_SIZE (320*240*2) // 根据实际分辨率调整

extern uint8_t g_image_buffer[OV7670_FRAME_SIZE];
extern uint8_t g_image_ready_flag;

void OV7670_Init(void);


#endif
