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
extern lv_obj_t *camera_img;
/**********************
 *      TYPEDEFS
 **********************/
lv_img_dsc_t camera_img_dsc = {
        .header.always_zero = 0,
        .header.w = CAMERA_WIDTH,
        .header.h = CAMERA_HEIGHT,
        .data_size = CAMERA_FRAME_SIZE,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = g_image_buffer,
    };
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
    /* Add your codes here */
}

