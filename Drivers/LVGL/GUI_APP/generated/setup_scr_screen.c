/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

#include "main.h"


void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes screen_img_1
    ui->image = lv_img_create(ui->screen);
    lv_obj_set_pos(ui->image, 0, 0);
    lv_obj_set_size(ui->image, CAMERA_WIDTH, CAMERA_HEIGHT);
    
    // 创建图片描述符并关联缓冲区
    static lv_img_dsc_t img_dsc = {
        .header.w = CAMERA_WIDTH,
        .header.h = CAMERA_HEIGHT,
        .header.cf = LV_IMG_CF_TRUE_COLOR,
        .data = g_image_buffer,
        .data_size = CAMERA_FRAME_SIZE
    };
    lv_img_set_src(ui->image, &img_dsc);
    
    lv_obj_clear_flag(ui->image, LV_OBJ_FLAG_CLICKABLE);

    //Update current screen layout.
    lv_obj_update_layout(ui->screen);

}