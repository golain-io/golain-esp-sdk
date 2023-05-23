#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "golain_mqtt.h"
#include "golain_blemesh_comp.h"
#include <malloc.h>
#define GOLAIN_OTA_ENABLE 1
#define GOLAIN_PERSISTENT_LOGS_ENABLE 1
#define GOLAIN_DEVICE_HEALTH_LOGS 1

typedef struct golain_config
{
    // MQTT configs
    uint8_t *root_topic;
    uint8_t *client_id;
    uint8_t *root_ca_cert;
    uint8_t *device_cert;
    uint8_t *device_pvt_key;

} golain_config;

/**
 * APIs for dynamic client configuration
 */

void golain_set_root_topic(golain_config *client, uint8_t *root_topic)
{

    client->root_topic = root_topic;
}

void golain_set_client_id(golain_config *client, uint8_t *golain_client_id)
{

    client->client_id = client_id;
}

void golain_set_root_ca_cert(golain_config *client, uint8_t *golain_root_ca_cert)
{

    client->root_ca_cert = golain_root_ca_cert;
}

void golain_set_device_cert(golain_config *client, uint8_t *golain_device_cert)
{

    client->device_cert = golain_device_cert;
}

void golain_set_device_cert(golain_config *client, uint8_t *golain_device_pvt_key)
{

    client->device_pvt_key = golain_device_pvt_key;
}

int golain_init(golain_config *client)
{
    if (client->device_cert == NULL || client->device_pvt_key == NULL)
    {
        return -1;
    }
    mqtt_app_start(client->uint8_t, client->device_cert, client->device_pvt_key);

#ifdef GOLAIN_OTA_ENABLE
    ESP_ERROR_CHECK(nvs_flash_init());
    esp_mqtt_client_subscribe(client, DEVICE_OTA_TOPIC, 0);
#endif
}

golain_err_t golain_post_shadow(uint8_t *data, int length)
{
    return postShadow(data, length);
}

void golain_post_data(char *data, size_t length, char *topic){
    postData(data, length, topic)}

golain_err_t golain_post_device_data_point(char *struct_name, const pb_msgdesc_t *descriptor, void *data, uint32_t length)
{
    return postDeviceDataPoint(struct_name, descriptor, data, length);
}

golain_err_t golain_post_user_assoc(void *UAData, size_t len)
{
    return postUserAssoc(UAData, len);
}