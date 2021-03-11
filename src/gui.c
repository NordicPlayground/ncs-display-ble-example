#include "gui.h"
#include <device.h>
#include <drivers/display.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(gui);

uint32_t count = 0U;
char count_str[11] = {0};
const struct device *display_dev;

static gui_event_t m_gui_event;
static gui_callback_t m_gui_callback = 0;

char *on_off_strings[2] = {"On", "Off"};

// GUI objects
lv_obj_t *top_header;
lv_obj_t *top_header_logo;
lv_obj_t *label_button, *label_led, *label_bt_state_hdr, *label_bt_state;
lv_obj_t *connected_background;
lv_obj_t *label_btn_state, *label_led_state;
lv_obj_t *btn1, *btn1_label;
lv_obj_t *checkbox_led;
lv_obj_t *image_led;
lv_obj_t *image_bg[12];

// Styles
lv_style_t style_btn, style_label, style_label_value, style_checkbox;
lv_style_t style_header, style_con_bg;

// Fonts
LV_FONT_DECLARE(arial_20bold);
LV_FONT_DECLARE(calibri_20b);
LV_FONT_DECLARE(calibri_20);
LV_FONT_DECLARE(calibri_24b);
LV_FONT_DECLARE(calibri_32b);

// Images
LV_IMG_DECLARE(nod_logo);
LV_IMG_DECLARE(led_on);
LV_IMG_DECLARE(led_off);
LV_IMG_DECLARE(img_noise_background);

static void gui_show_connected_elements(bool connected);

static void on_button1(lv_obj_t *btn, lv_event_t event)
{
	if(btn == btn1){
		if(event == LV_EVENT_PRESSED) {
			lv_label_set_text(label_btn_state, "Pressed");
			if(m_gui_callback) { 
				m_gui_event.evt_type = GUI_EVT_BUTTON_PRESSED;
				m_gui_event.button_checked = true;
				m_gui_callback(&m_gui_event);
			}
		}
		else if(event == LV_EVENT_RELEASED) {
			lv_label_set_text(label_btn_state, "Released");
			if(m_gui_callback) { 
				m_gui_event.evt_type = GUI_EVT_BUTTON_PRESSED;
				m_gui_event.button_checked = false;
				m_gui_callback(&m_gui_event);
			}
		}
	}
}

static void init_styles(void)
{
	/*Create background style*/
	static lv_style_t style_screen;
	lv_style_set_bg_color(&style_screen, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xcb, 0xca, 0xff));
	lv_obj_add_style(lv_scr_act(), LV_BTN_PART_MAIN, &style_screen);

	/*Create the screen header label style*/
	lv_style_init(&style_header);
	lv_style_set_bg_opa(&style_header, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x1C, 0x11, 0xFD));
	lv_style_set_radius(&style_header, LV_STATE_DEFAULT, 8);
	//lv_style_set_bg_grad_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_TEAL);
	//lv_style_set_bg_grad_dir(&style_header, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_pad_left(&style_header, LV_STATE_DEFAULT, 70);
	lv_style_set_pad_top(&style_header, LV_STATE_DEFAULT, 30);
	lv_style_set_shadow_spread(&style_header, LV_STATE_DEFAULT, 1);
	lv_style_set_shadow_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_BLACK);
	lv_style_set_shadow_opa(&style_header, LV_STATE_DEFAULT, 255);
	lv_style_set_shadow_width(&style_header, LV_STATE_DEFAULT, 1);
	lv_style_set_shadow_ofs_x(&style_header, LV_STATE_DEFAULT, 1);
	lv_style_set_shadow_ofs_y(&style_header, LV_STATE_DEFAULT, 2);
	lv_style_set_shadow_opa(&style_header, LV_STATE_DEFAULT, LV_OPA_50);

	/*Screen header text style*/
	lv_style_set_text_color(&style_header, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x7d, 0xce, 0xfd));
	lv_style_set_text_font(&style_header, LV_STATE_DEFAULT, &calibri_32b);
	

	lv_style_init(&style_con_bg);
	lv_style_copy(&style_con_bg, &style_header);
	lv_style_set_bg_color(&style_con_bg, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x69, 0xb0, 0x5a));
	lv_style_set_bg_opa(&style_con_bg, LV_STATE_DEFAULT, LV_OPA_50);
	lv_style_set_radius(&style_header, LV_STATE_DEFAULT, 4);
	
	/*Create a label style*/
	lv_style_init(&style_label);
	lv_style_set_bg_opa(&style_label, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_grad_dir(&style_label, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_pad_left(&style_label, LV_STATE_DEFAULT, 5);
	lv_style_set_pad_top(&style_label, LV_STATE_DEFAULT, 10);

	/*Add a border*/
	lv_style_set_border_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_WHITE);
	lv_style_set_border_opa(&style_label, LV_STATE_DEFAULT, LV_OPA_70);
	lv_style_set_border_width(&style_label, LV_STATE_DEFAULT, 3);

	/*Set the text style*/
	lv_style_set_text_color(&style_label, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x30));
	lv_style_set_text_font(&style_label, LV_STATE_DEFAULT, &calibri_20b);


	/*Create a label value style*/
	lv_style_init(&style_label_value);
	lv_style_set_bg_opa(&style_label_value, LV_STATE_DEFAULT, LV_OPA_20);
	lv_style_set_bg_color(&style_label_value, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_label_value, LV_STATE_DEFAULT, LV_COLOR_TEAL);
	lv_style_set_bg_grad_dir(&style_label_value, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_pad_left(&style_label_value, LV_STATE_DEFAULT, 0);
	lv_style_set_pad_top(&style_label_value, LV_STATE_DEFAULT, 3);

	/*Set the text style*/
	lv_style_set_text_color(&style_label_value, LV_STATE_DEFAULT, LV_COLOR_MAKE(0x00, 0x00, 0x30));
	lv_style_set_text_font(&style_label_value, LV_STATE_DEFAULT, &calibri_20);


	/*Create a simple button style*/
	lv_style_init(&style_btn);
	lv_style_set_radius(&style_btn, LV_STATE_DEFAULT, 10);
	lv_style_set_bg_opa(&style_btn, LV_STATE_DEFAULT, LV_OPA_COVER);
	lv_style_set_bg_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_SILVER);
	lv_style_set_bg_grad_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_bg_grad_dir(&style_btn, LV_STATE_DEFAULT, LV_GRAD_DIR_VER);
	lv_style_set_shadow_spread(&style_btn, LV_STATE_DEFAULT, 1);
	lv_style_set_shadow_color(&style_btn, LV_STATE_DEFAULT, LV_COLOR_GRAY);
	lv_style_set_shadow_opa(&style_btn, LV_STATE_DEFAULT, 255);
	lv_style_set_shadow_width(&style_btn, LV_STATE_DEFAULT, 1);

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
	lv_style_set_text_font(&style_btn, LV_STATE_DEFAULT, &calibri_24b);

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
}

static void init_blinky_gui(void)
{
	for(int i = 0; i < 12; i++){
		int x_index = i % 4;
		int y_index = i / 4;
		image_bg[i] = lv_img_create(lv_scr_act(), NULL);
		lv_obj_set_pos(image_bg[i], x_index*80, y_index*80);
		lv_obj_set_size(image_bg[i], 80, 80);
		lv_img_set_src(image_bg[i], &img_noise_background);		
	}

	// The connected header needs to be created before the top_header, to appear behind
	connected_background = lv_label_create(lv_scr_act(), NULL);
	lv_obj_add_style(connected_background, LV_LABEL_PART_MAIN, &style_con_bg);
	lv_label_set_long_mode(connected_background, LV_LABEL_LONG_DOT);
	lv_obj_set_pos(connected_background, 6, 65);
	lv_obj_set_size(connected_background, 308, 135);
	lv_label_set_text(connected_background, "");

	top_header = lv_label_create(lv_scr_act(), NULL);
	lv_obj_add_style(top_header, LV_LABEL_PART_MAIN, &style_header);
	lv_label_set_long_mode(top_header, LV_LABEL_LONG_DOT);
	lv_obj_set_pos(top_header, 3, 3);
	lv_obj_set_size(top_header, 314, 68);
	lv_label_set_text(top_header, "nRF Blinky");
	//lv_label_set_align(top_header, LV_LABEL_ALIGN_CENTER);

	top_header_logo = lv_img_create(lv_scr_act(), NULL);
	lv_obj_set_pos(top_header_logo, 0, 0);
	lv_obj_set_size(top_header_logo, 68, 68);
	lv_img_set_src(top_header_logo, &nod_logo);

	label_bt_state = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label_bt_state, LV_LABEL_LONG_CROP);
	lv_label_set_align(label_bt_state, LV_LABEL_ALIGN_CENTER); 
	lv_obj_set_pos(label_bt_state, 70, 210);
	lv_obj_set_size(label_bt_state, 180, 30);
	lv_label_set_text(label_bt_state, "Idle");
	lv_obj_add_style(label_bt_state, LV_LABEL_PART_MAIN, &style_label_value);

	btn1 = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
	lv_obj_set_pos(btn1, 10, 90);                            /*Set its position*/
	lv_obj_set_size(btn1, 120, 50);                          /*Set its size*/
	lv_obj_reset_style_list(btn1, LV_BTN_PART_MAIN);         /*Remove the styles coming from the theme*/
	lv_obj_add_style(btn1, LV_BTN_PART_MAIN, &style_btn);
	lv_btn_set_checkable(btn1, true);

	btn1_label = lv_label_create(btn1, NULL);          /*Add a label to the button*/
	lv_label_set_text(btn1_label, "Button");                     /*Set the labels text*/
	lv_obj_set_event_cb(btn1, on_button1);

	label_btn_state = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label_btn_state, LV_LABEL_LONG_DOT);
	lv_obj_set_pos(label_btn_state, 10, 170);
	lv_obj_set_size(label_btn_state, 120, 25);
	lv_label_set_text(label_btn_state, "Released");
	lv_label_set_align(label_btn_state, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(label_btn_state, LV_LABEL_PART_MAIN, &style_label_value);	

	image_led = lv_img_create(lv_scr_act(), NULL);
	lv_obj_set_pos(image_led, 192, 68);
	lv_obj_set_size(image_led, 68, 68);
	lv_img_set_src(image_led, &led_off);

	label_led_state = lv_label_create(lv_scr_act(), NULL);
	lv_label_set_long_mode(label_led_state, LV_LABEL_LONG_DOT);
	lv_obj_set_pos(label_led_state, 170, 170);
	lv_obj_set_size(label_led_state, 120, 25);
	lv_label_set_text(label_led_state, "Off");
	lv_label_set_align(label_led_state, LV_LABEL_ALIGN_CENTER);
	lv_obj_add_style(label_led_state, LV_LABEL_PART_MAIN, &style_label_value);

	gui_show_connected_elements(false);
}

static void gui_show_connected_elements(bool connected)
{
	lv_obj_set_hidden(connected_background, !connected);
	lv_obj_set_hidden(btn1, !connected);
	lv_obj_set_hidden(label_btn_state, !connected);
	lv_obj_set_hidden(image_led, !connected);
	lv_obj_set_hidden(label_led_state, !connected);
}

void gui_init(gui_config_t * config)
{
	m_gui_callback = config->event_callback;

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

void gui_set_bt_state(gui_state_t state)
{
	bool connected = false;
	switch(state)
	{
		case GUI_STATE_IDLE:
			lv_label_set_text(label_bt_state, "Idle");
			break;
		case GUI_STATE_ADVERTISING:
			lv_label_set_text(label_bt_state, "Advertising");
			break;
		case GUI_STATE_CONNECTED:
			lv_label_set_text(label_bt_state, "Connected");
			connected = true;
			break;
	}
	gui_show_connected_elements(connected);
}

void gui_set_bt_led_state(bool led_is_on)
{
	lv_img_set_src(image_led, led_is_on ? &led_on : &led_off);
	lv_label_set_text(label_led_state, led_is_on ? "On" : "Off");
}