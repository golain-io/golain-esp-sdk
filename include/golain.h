#ifndef _GOLAIN_H
#define _GOLAIN_H

typedef struct _golain_config_t
{
    void (*on_connect)(void);
    void (*on_disconnect)(void);
    void (*on_shadow_update_from_mqtt)(void);
    void (*on_data_publish)(void);

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
    void *mqtt;

    #ifdef CONFIG_GOLAIN_BLE
    void *ble;
    #endif

} golain_t;

#endif