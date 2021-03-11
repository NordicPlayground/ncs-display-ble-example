#ifndef __GUI_H
#define __GUI_H

#include <zephyr.h>

typedef enum {GUI_EVT_BUTTON_PRESSED} gui_event_type_t;

typedef enum {GUI_BT_STATE_IDLE, GUI_BT_STATE_ADVERTISING, GUI_BT_STATE_CONNECTED} gui_bt_state_t;

typedef struct
{
	gui_event_type_t evt_type;
	bool button_checked;
} gui_event_t;

typedef enum  {GUI_MSG_SET_STATE, GUI_MSG_SET_BT_STATE, GUI_MSG_SET_LED_STATE} gui_msg_type_t;

typedef struct
{
	gui_msg_type_t type;
	void *p_data;
	union 
	{
		gui_bt_state_t bt_state;
		bool led_state;
	} params;
} gui_message_t;


typedef void (*gui_callback_t)(gui_event_t *event);

typedef struct
{
	gui_callback_t event_callback;
} gui_config_t;

void gui_init(gui_config_t * config);

void gui_update(void);

void gui_set_bt_state(gui_bt_state_t state);

void gui_set_bt_led_state(bool led_on);

#endif
