#ifndef _GOLAIN_HAL_H
#define _GOLAIN_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "golain.h"
#include <stdio.h>

golain_err_t golain_hal_init(golain_t * _golain);

#ifdef CONFIG_GOLAIN_BLE
// ble functions
golain_err_t golain_hal_ble_init(golain_t * _golain);
uint8_t ble_addr_type;
void ble_app_advertise(void);

#endif
// mqtt functions
golain_err_t _golain_hal_mqtt_init(golain_t * _golain_client);

golain_err_t _golain_hal_mqtt_connect(void);

golain_err_t _golain_hal_mqtt_disconnect(void);

golain_err_t _golain_hal_mqtt_subscribe(const char *topic, uint8_t qos);

golain_err_t _golain_hal_mqtt_unsubscribe(const char *topic);

golain_err_t _golain_hal_mqtt_publish(const char *topic, const char *data, uint16_t data_len, uint8_t qos, bool retain);


// wifi functions
#ifdef CONFIG_GOLAIN_WIFI
golain_err_t _golain_hal_wifi_init(void);

golain_err_t _golain_hal_wifi_disconnect(void);
#endif

// shadow functions

golain_err_t _golain_hal_shadow_persistent_write(uint8_t * buff, size_t len);

golain_err_t _golain_hal_shadow_persistent_read(uint8_t * buff, size_t size);


/// @brief Encode the GLOBAL SHADOW into a buffer.data
golain_err_t _golain_hal_shadow_get(uint8_t * buff, size_t buff_len, size_t* encoded_size);

#ifdef __cplusplus
}
#endif

#endif