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
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "lvgl.h"
#include "custom.h"
#include "../../GUI/lvgl/src/draw/lv_img_buf.h"   // lv_img_dsc_t 定义
#include "../../GUI/lvgl/src/widgets/lv_img.h"     // 包含 lv_img_invalidate 声明
#include "../../GUI/lvgl/src/misc/lv_mem.h"        // lv_mem_alloc/free 定义
#include "bsp_ov7670.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void ov7670_to_lvgl_format(uint8_t *src, uint8_t *dest, uint32_t len);
static void vCameraTask(void *argument);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_img_dsc_t *current_img_dsc = NULL;
static TaskHandle_t camera_task_handle = NULL;
static SemaphoreHandle_t camera_mutex;

volatile uint8_t g_capturing = 0; 

lv_img_dsc_t camera_img_dsc = { 
    .header.w = CAMERA_WIDTH,
    .header.h = CAMERA_HEIGHT,
    .header.cf = LV_IMG_CF_RGB565,
    .data_size = CAMERA_FRAME_SIZE,    
    .data = g_image_buffer,                  
};

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void custom_start_camera_preview(lv_img_dsc_t *img_dsc)
{
    if(img_dsc == NULL) return;
    
    static bool camera_inited = false;
    if(!camera_inited) {
        OV7670_Init();
        camera_mutex = xSemaphoreCreateMutex();
        camera_inited = true;
    }
    
    img_dsc->data = (uint8_t *)lv_mem_alloc(CAMERA_FRAME_SIZE);
    img_dsc->data_size = CAMERA_FRAME_SIZE;
    img_dsc->header.cf = LV_IMG_CF_RGB565;
    img_dsc->header.w = CAMERA_WIDTH;
    img_dsc->header.h = CAMERA_HEIGHT;
    
    current_img_dsc = img_dsc;
    g_capturing = 1;  // 使用1表示true
}

void custom_stop_camera_preview(void)
{
    g_capturing = 0;  
    
    if(current_img_dsc) {
        if(xSemaphoreTake(camera_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            if(current_img_dsc->data) {
                lv_mem_free((void *)current_img_dsc->data);
                current_img_dsc->data = NULL;
                current_img_dsc->data_size = 0;
            }
            xSemaphoreGive(camera_mutex);
        }
        current_img_dsc = NULL;
    }
    
    if(camera_task_handle) {
        vTaskDelete(camera_task_handle);
        camera_task_handle = NULL;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/
