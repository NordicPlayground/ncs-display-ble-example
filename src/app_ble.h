#ifndef __APP_BLE_H
#define __APP_BLE_H

#include <zephyr.h>

typedef enum {APP_BLE_CONNECTED, APP_BLE_DISCONNECTED, APP_BLE_ON_LED_WRITE} app_ble_event_type_t;

typedef struct
{
	app_ble_event_type_t type;
	bool led_state;
} app_ble_event_data_t;

typedef void (*app_ble_event_t)(app_ble_event_data_t * evt_data);

typedef struct
{
	app_ble_event_t event_callback;
} app_ble_config_t;

void app_ble_init(app_ble_config_t * config);

void app_ble_start_advertising(void);

void app_ble_send_button_state(bool btn_pressed);

#endif
