/*
 * Copyright (c) 2023 Quoppo LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _GOLAIN_HAL_H
#define _GOLAIN_HAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "golain.h"
#include "esp_log.h"
#include "esp_err.h"
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



// ota funnctions
#ifdef CONFIG_GOLAIN_MQTT_OTA
  golain_err_t _golain_hal_ota_update(int event_total_data_len, char* event_data, int event_data_len);
#endif

// persistent log functions
/**
 * @brief - Logs and stores the Info statements given as a param
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */
#ifdef CONFIG_GOLAIN_CLOUD_LOGGING
  #define GOLAIN_LOG_I(tag, format, ...) \
    ESP_LOGI(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
    _golain_hal_p_log_write(ESP_LOG_INFO, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#else
  #define GOLAIN_LOG_I(tag, format, ...) \
    ESP_LOGI(tag, "(%s)-> " format, __func__, ##__VA_ARGS__);
#endif



/**
 * @brief - Logs and stores the Warning statements given as a param
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */  
#ifdef CONFIG_GOLAIN_CLOUD_LOGGING
  #define GOLAIN_LOG_W(tag, format, ...) \
    ESP_LOGW(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
    _golain_hal_p_log_write(ESP_LOG_WARN, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#else
  #define GOLAIN_LOG_W(tag, format, ...) \
    ESP_LOGW(tag, "(%s)-> " format, __func__, ##__VA_ARGS__);
#endif

/**
 * @brief 
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */

#define GOLAIN_LOG_D(tag, format, ...) \
    ESP_LOGD(tag, "(%s)-> " format, __func__, ##__VA_ARGS__);


#ifdef CONFIG_GOLAIN_CLOUD_LOGGING
  #define GOLAIN_LOG_E(tag, format, ...) \
    ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
    _golain_hal_p_log_write(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#else
  #define GOLAIN_LOG_E(tag, format, ...) \
    ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__);
#endif


#ifdef CONFIG_GOLAIN_CLOUD_LOGGING

golain_err_t _golain_hal_p_log_check_nvs_errors(esp_err_t err);

golain_err_t _golain_hal_p_log_write_to_nvs(uint8_t *data, size_t len);

golain_err_t _golain_hal_p_log_write(esp_log_level_t level, const char *func, const char *tag, const char *format, ...);

golain_err_t _golain_hal_p_log_read_old_logs(uint8_t *buffer);

golain_err_t _golain_hal_p_log_get_number_of_logs(int* num);

#endif


#ifdef CONFIG_GOLAIN_DEVICE_HEALTH

golain_err_t _golain_hal_device_health_store(uint8_t *deviceHealthproto); 
int8_t _golain_hal_reset_counter(void);

#endif

#ifdef __cplusplus
}
#endif

#endif