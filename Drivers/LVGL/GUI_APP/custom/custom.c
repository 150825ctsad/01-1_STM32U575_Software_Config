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
#include "lvgl.h"
#include "custom.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
lv_img_dsc_t camera_img_dsc = {
    .header.always_zero = 0,
    .header.w = CAMERA_WIDTH,
    .header.h = CAMERA_HEIGHT,
    .header.cf = LV_IMG_CF_RGB565,
    .data_size = CAMERA_FRAME_SIZE,
    .data = NULL // Will be set dynamically
};

lv_obj_t *camera_img;
/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    camera_img = lv_img_create(ui->screen);
    lv_img_set_src(camera_img, &camera_img_dsc);
}

