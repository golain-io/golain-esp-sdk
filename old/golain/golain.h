/*
 * Copyright (c) 2023 Quoppo Inc.
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

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "golain_mqtt.h"
#include "golain_blemesh_comp.h"
#include "shadow_helper.h"
#include "golain_err.h"
#include <malloc.h>
#define GOLAIN_OTA_ENABLE 1
#define GOLAIN_PERSISTENT_LOGS_ENABLE 1
#define GOLAIN_DEVICE_HEALTH_LOGS 1

typedef struct golain_config
{
    // MQTT configs
    uint8_t *root_topic;
    uint8_t *client_id;
 #if CONFIG_HANDLE_WIFI == y
    uint8_t *wifissid;
    uint8_t *wifipass;
#endif
    uint8_t *root_ca_cert; 
    uint8_t *device_cert;
    uint8_t *device_pvt_key;

    ShadowCfg shadowcfg;

#if GOLAIN_BLE_ENABLE == y
    //BLE Config
    MeshConfig bleconfig;
#endif




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

// void golain_set_root_ca_cert(golain_config *client, uint8_t *golain_root_ca_cert)
// {

//     client->root_ca_cert = golain_root_ca_cert;
// }

// void golain_set_device_cert(golain_config *client, uint8_t *golain_device_cert)
// {

//     client->device_cert = golain_device_cert;
// }

// void golain_set_device_cert(golain_config *client, uint8_t *golain_device_pvt_key)
// {

//     client->device_pvt_key = golain_device_pvt_key;
// }

int golain_init(golain_config *client)
{
    if (client->device_cert == NULL || client->device_pvt_key == NULL)
    {
        return -1;
    }
    mqtt_app_start(client->client_id, client->device_cert, client->device_pvt_key);

    //Shadow needs to be init for mqtt
    InitDeviceShadow(client->shadowcfg);

#ifdef CONFIG_HANDLE_WIFI
    wifi_init_sta(client->wifissid, client->wifipass);
#endif

#ifdef GOLAIN_BLE_ENABLE
    bt_mesh_custom_start(client->bleconfig);
#endif 
}

golain_err_t golain_post_shadow(uint8_t *data, int length)
{
    return postShadow(data, length);
}

void golain_post_data(char *data, size_t length, char *topic){ //Not needed
    postData(data, length, topic)
    }

golain_err_t golain_post_device_data_point(char *struct_name, const pb_msgdesc_t *descriptor, void *data, uint32_t length)
{
    return postDeviceDataPoint(struct_name, descriptor, data, length);
}

golain_err_t golain_post_user_assoc(void *UAData, size_t len)
{
    return postUserAssoc(UAData, len);
}

golain_err_t golain_send_ble_msg(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf,
                           uint8_t * buffer,
                           size_t len){

                            return send_message(model, ctx, buf, buffer, len);
                           }

golain_err_t golain_get_shadow(uint8_t * buff, size_t buff_len, size_t* encoded_size){
    return GetShadow(buff, buff_len, encoded_size);
}

golain_err_t golain_update_with_struct(void){
    return UpdatewithStruct();
}

golain_err_t golain_update_with_buff(uint8_t * buff, size_t len){
    return UpdatewithBuff(buff, len);
}
