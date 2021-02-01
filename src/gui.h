#ifndef __GUI_H
#define __GUI_H

#include <zephyr.h>

typedef enum {GUI_EVT_BUTTON_PRESSED} gui_event_type_t;

typedef struct
{
	gui_event_type_t evt_type;
	bool button_checked;
} gui_event_t;


typedef void (*gui_callback_t)(gui_event_t *event);

typedef struct
{
	gui_callback_t event_callback;
} gui_config_t;

void gui_init(gui_config_t * config);

void gui_update(void);

void gui_set_bt_state(const char * bt_state_str);

void gui_set_bt_led_state(bool led_on);

#endif
