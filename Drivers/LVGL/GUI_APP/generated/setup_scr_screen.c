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
#include "bsp_ov7670.h"

// 声明屏幕组件
static lv_obj_t *screen_label;
static lv_obj_t *screen_btn;
static lv_obj_t *camera_ctrl_btn;  // 摄像头控制按钮
static lv_obj_t *camera_back_btn;  // 返回按钮
static lv_obj_t *camera_img;       // 摄像头图像控件

// 图像描述符（保留，用于占位）
lv_img_dsc_t camera_img_dsc = {
    .header.w = 320,                  // 与摄像头分辨率一致
    .header.h = 240,
};

// 按钮点击计数回调（原有功能，保留）
static void screen_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        static uint32_t click_count = 0;
        click_count++;
        char buf[32];
        sprintf(buf, "Clicked: %d times", click_count);
        lv_label_set_text(screen_label, buf);
    }
}

// 摄像头控制按钮回调（核心功能，保留）
static void camera_ctrl_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // 隐藏原始控件
        lv_obj_add_flag(screen_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(screen_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(camera_ctrl_btn, LV_OBJ_FLAG_HIDDEN);
        
        // 显示摄像头画面和返回按钮
        lv_obj_clear_flag(camera_img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(camera_back_btn, LV_OBJ_FLAG_HIDDEN);

        g_capturing = 1;
        //printf("%d\n",g_capturing);
    }
}

// 摄像头画面返回按钮回调（保留）
static void camera_back_btn_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // 隐藏摄像头相关控件
        lv_obj_add_flag(camera_img, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(camera_back_btn, LV_OBJ_FLAG_HIDDEN);
        
        // 显示原始控件
        lv_obj_clear_flag(screen_label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(screen_btn, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(camera_ctrl_btn, LV_OBJ_FLAG_HIDDEN);
        
        // 停止摄像头采集
        g_capturing = 0;
        //printf("%d\n",g_capturing);
    }
}


void setup_scr_screen(lv_ui *ui) {
    // 创建主屏幕（保留）
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_color(ui->screen, lv_color_hex(0x003a6e), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建容器（保留）
    ui->screen_cont_1 = lv_obj_create(ui->screen);
    lv_obj_set_pos(ui->screen_cont_1, 0, 0);
    lv_obj_set_size(ui->screen_cont_1, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen_cont_1, LV_SCROLLBAR_MODE_OFF);
    
    // 设置容器样式（保留）
    lv_obj_set_style_border_width(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->screen_cont_1, lv_color_hex(0x0055a5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->screen_cont_1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建标题标签（保留）
    lv_obj_t *title_label = lv_label_create(ui->screen_cont_1);
    lv_label_set_text(title_label, "Camera Control");
    lv_obj_set_pos(title_label, 100, 10);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建状态标签（原有，保留）
    screen_label = lv_label_create(ui->screen_cont_1);
    lv_label_set_text(screen_label, "Click the button!");
    lv_obj_set_pos(screen_label, 80, 80);
    lv_obj_set_style_text_color(screen_label, lv_color_hex(0xffffff), LV_PART_MAIN | LV_STATE_DEFAULT);

    // 创建交互按钮（原有，保留）
    screen_btn = lv_btn_create(ui->screen_cont_1);
    lv_obj_set_pos(screen_btn, 110, 130);
    lv_obj_set_size(screen_btn, 80, 40);
    lv_obj_add_event_cb(screen_btn, screen_btn_event_cb, LV_EVENT_ALL, NULL);
    
    // 按钮文本（原有，保留）
    lv_obj_t *btn_label = lv_label_create(screen_btn);
    lv_label_set_text(btn_label, "Click Me");
    lv_obj_center(btn_label);

    // 创建摄像头控制按钮
    camera_ctrl_btn = lv_btn_create(ui->screen_cont_1);
    lv_obj_set_pos(camera_ctrl_btn, 110, 190);  // 底部位置
    lv_obj_set_size(camera_ctrl_btn, 100, 30);
    lv_obj_add_event_cb(camera_ctrl_btn, camera_ctrl_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    // 摄像头按钮文本（保留）
    lv_obj_t *camera_btn_label = lv_label_create(camera_ctrl_btn);
    lv_label_set_text(camera_btn_label, "Start Camera");
    lv_obj_center(camera_btn_label);

    // 创建摄像头图像控件（初始隐藏，保留）
    camera_img = lv_img_create(ui->screen_cont_1);
    lv_img_set_src(camera_img, &camera_img_dsc);  // 关联图像描述符（占位）
    lv_obj_set_pos(camera_img, 0, 0);             // 全屏显示
    lv_obj_set_size(camera_img, 320, 240);
    lv_obj_add_flag(camera_img, LV_OBJ_FLAG_HIDDEN);  // 初始隐藏

    // 创建返回按钮（初始隐藏，保留）
    camera_back_btn = lv_btn_create(ui->screen_cont_1);
    lv_obj_set_pos(camera_back_btn, 10, 100);  // 左上角
    lv_obj_set_size(camera_back_btn, 60, 25);
    lv_obj_add_event_cb(camera_back_btn, camera_back_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_flag(camera_back_btn, LV_OBJ_FLAG_HIDDEN);  // 初始隐藏
    
    // 返回按钮文本（保留）
    lv_obj_t *back_btn_label = lv_label_create(camera_back_btn);
    lv_label_set_text(back_btn_label, "Back");
    lv_obj_center(back_btn_label);

    // 更新布局（保留）
    lv_obj_update_layout(ui->screen);
}
