#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

#include "bsp_ov2640.h"

void setup_scr_screen(lv_ui *ui)
{
    //Write codes screen
    ui->screen = lv_obj_create(NULL);
    lv_obj_set_size(ui->screen, 320, 240);
    lv_obj_set_scrollbar_mode(ui->screen, LV_SCROLLBAR_MODE_OFF);

    //Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes image - 添加图片组件
    ui->image = lv_img_create(ui->screen);
    lv_obj_set_pos(ui->image, 0, 0);
    lv_obj_set_size(ui->image, 160, 120);
    lv_img_set_zoom(ui->image, 256);  // 100% 缩放
    lv_img_set_pivot(ui->image, 0, 0); // 设置旋转中心

    // 初始化图片为空图片或默认图片
    lv_img_set_src(ui->image, NULL);

    lv_obj_update_layout(ui->screen);
}