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

char *on_off_strings[2] = {"On", "Off"};

// GUI objects
lv_obj_t * label_button, * label_led, * label_bt_state_hdr, * label_bt_state;
lv_obj_t * btn1, * btn1_label;
lv_obj_t * checkbox_led;

// Styles
lv_style_t style_btn, style_label, style_checkbox;

// Fonts
LV_FONT_DECLARE(arial_20b);

static void on_button1(lv_obj_t *btn, lv_event_t event)
{
	static bool button_pressed = false;
	if(event == LV_EVENT_CLICKED) {
		if(btn == btn1){
			printk("Button 1 pressed\n");
			button_pressed = !button_pressed;
			lv_label_set_text(btn1_label, on_off_strings[button_pressed ? 1 : 0]);
		}
	}
}

static void init_styles(void)
{
	/*Create background style*/
	static lv_style_t style_screen;
	lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x30));
	lv_obj_add_style(lv_scr_act(), LV_BTN_PART_MAIN, &style_screen);


	/*Create a label style*/
	lv_style_init(&style_label);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 5);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 5);

	/*Add a border*/
	lv_style_set_border_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_border_opa(&style_label, LV_STATE_DEFAULT, LV_OPA_70);
	lv_style_set_border_width(&style_label, LV_STATE_DEFAULT, 3);

	/*Set the text style*/
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x30));
	lv_style_set_text_font(&style_label, LV_STATE_DEFAULT, &arial_20b);


	/*Create a checkbox style*/
	lv_style_init(&style_checkbox);
	lv_style_set_bg_opa(&style_checkbox, LV_STATE_DEFAULT, LV_OPA_20);
	lv_style_set_bg_color(&style_checkbox, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_checkbox, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_grad_dir(&style_checkbox, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_pad_left(&style_checkbox, LV_STATE_DEFAULT, 5);
	lv_style_set_pad_top(&style_checkbox, LV_STATE_DEFAULT, 5);

	/*Set the text style*/
	lv_style_set_text_color(&style_checkbox, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x30));
	lv_style_set_text_font(&style_checkbox, LV_STATE_DEFAULT, &arial_20b);

	lv_style_set_outline_color(&style_checkbox, LV_STATE_DEFAULT, LV_COLOR_WHITE);


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
	lv_style_set_border_width(&style_btn, LV_STATE_DEFAULT, 3);

	/*Different border color in focused state*/
	lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED, LV_COLOR_BLACK);
	lv_style_set_border_color(&style_btn, LV_STATE_FOCUSED | LV_STATE_PRESSED, LV_COLOR_NAVY);

	/*Set the text style*/
	lv_style_set_text_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_TEAL);
	lv_style_set_text_font(&style_btn, LV_STATE_DEFAULT, &arial_20b);

	/*Make the button smaller when pressed*/
	lv_style_set_transform_height(&style_btn, LV_STATE_PRESSED, -4);
	lv_style_set_transform_width(&style_btn, LV_STATE_PRESSED, -8);
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

	/*Create buttons and use the new styles*/
	btn1 = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
	lv_obj_set_pos(btn1, 10, 60);                            /*Set its position*/
	lv_obj_set_size(btn1, 120, 50);                          /*Set its size*/
	lv_obj_reset_style_list(btn1, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
	lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style_btn);
	lv_btn_set_checkable(btn1, true);
	//lv_obj_set_hidden(btn1, true);

	btn1_label = lv_label_create(btn1, NULL);          /*Add a label to the button*/
	lv_label_set_text(btn1_label, on_off_strings[0]);                     /*Set the labels text*/

	lv_obj_set_event_cb(btn1, on_button1);
}

static void init_blinky_gui(void)
{
	label_button = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label_button, LV_LABEL_LONG_DOT);
	lv_obj_set_pos(label_button, 5, 5);
	lv_obj_set_size(label_button, 150, 40);
	lv_label_set_text(label_button, "Button");
	lv_obj_add_style(label_button, LV_LABEL_PART_MAIN, &style_label);

	label_led = lv_label_create(lv_scr_act(), label_button);
	lv_obj_set_pos(label_button, 165, 5);
	lv_obj_set_size(label_button, 150, 40);
	lv_label_set_text(label_button, "LED");

	label_bt_state_hdr = lv_label_create(lv_scr_act(), label_button);
	lv_obj_set_pos(label_bt_state_hdr, 5, 160);
	lv_obj_set_size(label_bt_state_hdr, 200, 40);
	lv_label_set_text(label_bt_state_hdr, "Bluetooth state");

	label_bt_state = lv_label_create(lv_scr_act(), label_button);
	lv_obj_set_pos(label_bt_state, 5, 200);
	lv_obj_set_size(label_bt_state, 200, 40);
	lv_label_set_text(label_bt_state, "Advertising");

	checkbox_led = lv_checkbox_create(lv_scr_act(), NULL);
	lv_obj_set_pos(checkbox_led, 165, 75);
	lv_obj_set_size(checkbox_led, 50, 50);
	lv_checkbox_set_text(checkbox_led, "");
	lv_checkbox_set_checked(checkbox_led, true);
	lv_obj_add_style(checkbox_led, LV_LABEL_PART_MAIN, &style_label);
	
}

void gui_init(void)
{
	display_dev = device_get_binding(CONFIG_LVGL_DISPLAY_DEV_NAME);

	if (display_dev == NULL) {
		LOG_ERR("device not found.  Aborting test.");
		return;
	}

	init_styles();

	init_blinky_gui();

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

void gui_set_bt_state(const char * bt_state_str)
{
	lv_label_set_text(label_bt_state, bt_state_str);
}

void gui_set_bt_led_state(bool led_on)
{
	lv_checkbox_set_checked(checkbox_led, led_on);
}