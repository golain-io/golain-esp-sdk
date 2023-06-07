#ifndef _GOLAIN_TYPES_H
#define _GOLAIN_TYPES_H

#include <stdio.h>
#include <stdbool.h>
#include "sdkconfig.h"

typedef enum _golain_err_t
{
    GOLAIN_OK = 0,    /*!< esp_err_t value indicating success (no error) */
    GOLAIN_FAIL = -1, /*!< Generic esp_err_t code indicating failure */
    NVS_NOT_INIT = 0x0001, 
    NVS_UPDATE_FAIL = 0x0002,
    NVS_READ_FAIL = 0x0003,
    NVS_EMPTY = 0x0004,
    PB_UPDATE_FAIL =  0x0005,
    PB_ENCODE_FAIL = 0x0006,


    GOLAIN_ERR_NO_MEM = 0x1001,           /*!< Out of memory */
    GOLAIN_ERR_INVALID_ARG = 0x1002,      /*!< Invalid argument */
    GOLAIN_ERR_INVALID_STATE = 0x1003,    /*!< Invalid state */
    GOLAIN_ERR_INVALID_SIZE = 0x1004,     /*!< Invalid size */

    GOLAIN_ERR_NOT_FOUND = 0x1005,        /*!< Requested resource not found */
    GOLAIN_ERR_NOT_SUPPORTED = 0x1006,    /*!< Operation or feature not supported */
    GOLAIN_ERR_TIMEOUT = 0x1007,          /*!< Operation timed out */
    GOLAIN_ERR_INVALID_RESPONSE = 0x1008, /*!< Received response was invalid */
    GOLAIN_ERR_INVALID_CRC = 0x1009,      /*!< CRC or checksum was invalid */
    GOLAIN_ERR_NOT_FINISHED = 0x100C,     /*!< There are items remained to retrieve */
    
    GOLAIN_ERR_INVALID_VERSION = 0x100A,  /*!< Version was invalid */
    GOLAIN_ERR_INVALID_MAC = 0x100B,      /*!< MAC address was invalid */

    GOLAIN_ERR_WIFI_BASE = 0x2000,      /*!< Starting number of WiFi error codes */
    GOLAIN_ERR_MESH_BASE = 0x3000,      /*!< Starting number of BLE error codes */
    GOLAIN_ERR_FLASH_BASE = 0x4000,     /*!< Starting number of NVS error codes */
    
    GOLAIN_MQTT_CONNECT_FAIL = 0x5001,
    GOLAIN_MQTT_DISCONNECT_FAIL = 0x5002,
    GOLAIN_MQTT_SUBSCRIBE_FAIL = 0x5003,
    GOLAIN_MQTT_PUBLISH_FAIL = 0x5004,
    GOLAIN_MQTT_UNSUBSCRIBE_FAIL = 0x5005,

} golain_err_t;

typedef struct _golain_config_t
{
    void (*on_connect)(void);
    void (*on_disconnect)(void);
    void (*on_shadow_update_from_mqtt)(void);
    void (*on_data_publish)(void);
    void * shadow_struct;
    const void * shadow_fields;
    size_t shadow_size;

    // mqtt certs
    const void * device_cert;
    const void * device_pvt_key;
    const void * root_ca_cert_start;
    const unsigned int root_ca_cert_len;

    #ifdef CONFIG_GOLAIN_BLE
    void (*on_shadow_update_from_ble)(void);
    #endif

    #ifdef CONFIG_GOLAIN_MQTT_OTA
    void (*on_ota_payload_received)(void *payload, uint32_t payload_len);
    #endif

} golain_config_t;

typedef struct _golain_t
{
    golain_config_t *config;
    bool mqtt_is_connected;

    #ifdef CONFIG_GOLAIN_BLE
    void *ble;
    #endif

} golain_t;

golain_err_t golain_hal_init(golain_t * _golain);

uint8_t shadow_buffer[CONFIG_GOLAIN_SHADOW_BUFFER_SIZE];

#ifdef CONFIG_GOLAIN_BLE
// ble functions
golain_err_t golain_hal_ble_init(golain_t * _golain);
uint8_t ble_addr_type;
void ble_app_advertise(void);

#endif


#endif