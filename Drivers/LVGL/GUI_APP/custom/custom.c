/*
* Copyright 2023 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
lv_img_dsc_t camera_img_dsc = {
    .header.w = 320,                  // 与摄像头分辨率一致
    .header.h = 240,
    .header.cf = LV_IMG_CF_RGB565,    // 与摄像头输出格式一致（RGB565）
    .data_size = OV7670_FRAME_SIZE,   // 320*240*2（由 bsp_ov7670.h 定义）
    .data = g_image_buffer,           // 指向摄像头采集的全局缓冲区
};

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

