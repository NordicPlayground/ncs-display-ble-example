#include "gui.h"
#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(gui);

uint32_t count = 0U;
char count_str[11] = {0};
const struct device *display_dev;
lv_obj_t * btn1, * btn2;

// Styles
lv_style_t style_btn;

// Fonts
LV_FONT_DECLARE(arial_20b);

static void on_button1(lv_obj_t *btn, lv_event_t event)
{
	if(event == LV_EVENT_CLICKED) {
		if(btn == btn1){
			printk("Button 1 pressed\n");
		}
		else if(btn == btn2){
			printk("Button 2 pressed\n");
		}
	}
}

static void init_styles(void)
{
	/*Create a simple button style*/
	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, LV_STATE_DEFAULT, 10);
	lv_style_set_bg_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_grad_dir(&style_btn, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);

	/*Swap the colors in pressed state*/
	lv_style_set_bg_color(&style_btn, LV_STATE_PRESSED, LV_COLOR_GRAY);
	lv_style_set_bg_grad_color(&style_btn, LV_STATE_PRESSED, LV_COLOR_SILVER);

	/*Add a border*/
	lv_style_set_border_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_border_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_70);
	lv_style_set_border_width(&style_btn, LV_STATE_DEFAULT, 2);

	/*Different border color in focused state*/
	lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED, LV_COLOR_BLUE);
	lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED | LV_STATE_PRESSED, LV_COLOR_NAVY);

	/*Set the text style*/
	lv_style_set_text_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_TEAL);
	lv_style_set_text_font(&style_btn, LV_STATE_DEFAULT, &arial_20b);

	/*Make the button smaller when pressed*/
	lv_style_set_transform_height(&style_btn, LV_STATE_PRESSED, -2);
	lv_style_set_transform_width(&style_btn, LV_STATE_PRESSED, -4);
#if LV_USE_ANIMATION
	/*Add a transition to the size change*/
	static lv_anim_path_t path;
	lv_anim_path_init(&path);
	lv_anim_path_set_cb(&path, lv_anim_path_overshoot);

	lv_style_set_transition_prop_1(&style_btn, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_HEIGHT);
	lv_style_set_transition_prop_2(&style_btn, LV_STATE_DEFAULT, LV_STYLE_TRANSFORM_WIDTH);
	lv_style_set_transition_time(&style_btn, LV_STATE_DEFAULT, 300);
	lv_style_set_transition_path(&style_btn, LV_STATE_DEFAULT, &path);
#endif

#if LV_USE_BTN
	/*Create buttons and use the new styles*/
	btn1 = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
	lv_obj_set_pos(btn1, 10, 10);                            /*Set its position*/
	lv_obj_set_size(btn1, 120, 50);                          /*Set its size*/
	lv_obj_reset_style_list(btn1, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
	lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style_btn);

	lv_obj_t * label = lv_label_create(btn1, NULL);          /*Add a label to the button*/
	lv_label_set_text(label, "Button");                     /*Set the labels text*/
	lv_obj_set_event_cb(btn1, on_button1);

	/*Create a new button*/
	btn2 = lv_btn_create(lv_scr_act(), btn1);
	lv_obj_set_pos(btn2, 10, 80);
	lv_obj_set_size(btn2, 120, 50);                             /*Set its size*/
	lv_obj_reset_style_list(btn2, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
	lv_obj_add_style(btn2, LV_BTN_PART_MAIN, &style_btn);

	label = lv_label_create(btn2, NULL);          /*Add a label to the button*/
	lv_label_set_text(label, "Button 2");                     /*Set the labels text*/
#endif
}

void gui_init(void)
{
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	init_styles();

	lv_task_handler();
	display_blanking_off(display_dev);
}

void gui_update(void)
{
	if ((count % 100) == 0U) {

	}
	lv_task_handler();
	++count;
}