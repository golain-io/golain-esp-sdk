#ifndef _GOLAIN_H
#define _GOLAIN_H

#include <stdio.h>
#include <stdbool.h>
#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#define DATA_TOPIC(name) GOLAIN_DATA_TOPIC name

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

uint8_t shadow_buffer[CONFIG_GOLAIN_SHADOW_BUFFER_SIZE];

typedef struct _golain_t
{
    golain_config_t *config;
    bool mqtt_is_connected;

    #ifdef CONFIG_GOLAIN_BLE
    void *ble;
    #endif

} golain_t;

golain_err_t golain_init(golain_t *golain, golain_config_t *config);


/*-------------------------------------------------------MQTT-------------------------------------*/
golain_err_t golain_mqtt_process_message(golain_t* _golain, char* topic, size_t topic_len, char * data, size_t data_len, size_t total_len);

golain_err_t golain_mqtt_post_data_point(char* topic, const void* descriptor, void * data, uint32_t length);
golain_err_t golain_mqtt_post_shadow(golain_t*);

#ifdef CONFIG_GOLAIN_CONSUMER_ENABLED
golain_err_t golain_mqtt_post_user_assoc(golain_t*, uint8_t * buff, uint8_t len);
#endif

/*-------------------------------------------------------WIFI-------------------------------------*/
#ifdef CONFIG_GOLAIN_WIFI
golain_err_t golain_hal_wifi_init();
golain_err_t golain_hal_wifi_disconnect(void);
#endif

/*-------------------------------------------------------Shadow-----------------------------------*/
/// @brief Initialises the global device shadow from NVS. If it does not find a shadow, default state is use.
/// @param  Pointer to the golain config struct
/// @return OK if successful, otherwise an error code
golain_err_t golain_shadow_init(golain_t*);

/// @brief Updates the GLOBAL SHADOW with a received buffer 
/// @param buff Received buffer conatining the encoded device shadow
/// @param len Length of `buff`
/// @return GOLAIN_OK on success, error code on failure.
golain_err_t _golain_shadow_update_from_buffer(golain_t* _golain, uint8_t * buff, size_t len);


/// @brief Updates shadow buffer and NVS with the GLOBAL SHADOW
/// @return GOLAIN_OK if successful, otherwise, error codes will be received
golain_err_t golain_shadow_update(golain_t*);


golain_err_t _golain_shadow_get_trimmed_shadow_buffer(golain_t *, size_t* );

/*------------------------------------------------------Persistent Logs-------------------------------------*/


/*------------------------------------------------------Device Health---------------------------------------*/
#ifdef CONFIG_GOLAIN_REPORT_DEVICE_HEALTH
golain_err_t golain_device_health_encode_message(uint8_t *buffer, size_t buffer_size, size_t *message_length);


golain_err_t golain_device_health_decode_message(uint8_t *buffer, size_t message_length);



#endif
/*------------------------------------------------------Nanopb callbacks-------------------------------------*/
bool golain_pb_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

bool golain_pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) ;


#endif