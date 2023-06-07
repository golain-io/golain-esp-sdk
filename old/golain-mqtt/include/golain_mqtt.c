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

#include "golain_mqtt.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
#include "pb.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
//#include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>
#include "shadow_helper.h"
#include "shadow.pb.h"






//-----------------------------------------------------------------------------------------------------------Only  used in this file


void splitintoarray(char * from_string, char ** str_array, char * split_char){
    char * temp_array = from_string;
    //memset(str_array[0], 0x00, 10);
    //memset(str_array[1], 0x00, 10);

    str_array[0] = strtok(temp_array, split_char);
    for(int i = 0; i< 3-1; i++){
        str_array[i+1] = strtok(NULL, split_char);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    _golain_mqtt_client = event->client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        

        
        msg_id = esp_mqtt_client_subscribe(_golain_mqtt_client, DEVICE_SHADOW_TOPIC_R, 0);
        ESP_LOGI(TAG, "Sent subscribe successful, msg_id=%d", msg_id);

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);

        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        
        
        if(strncmp(event->topic, DEVICE_SHADOW_TOPIC_U, event->topic_len)==0){
            
            uint8_t shadowreadbuff[Shadow_size];
            pb_ostream_t ostream = pb_ostream_from_buffer(shadowreadbuff, Shadow_size);
            pb_encode(&ostream, Shadow_fields, &shadow);
            postShadow(shadowreadbuff, ostream.bytes_written);

        }
        else if(strncmp(event->topic, DEVICE_SHADOW_TOPIC_R, event->topic_len)==0){
            uint8_t shadowwritebuff[event->data_len];
            memcpy(shadowwritebuff, event->data, event->data_len);
            UpdatewithBuff(shadowwritebuff, event->data_len);
        }
        else{
        dataRcvFlag = 1;
        memcpy(dirtyTopicArray, event->topic, event->topic_len);
        rcvTopicLength = event->topic_len;

        memcpy(dirtyDataArray, event->data, event->data_len);
        rcvDataLength = event->data_len;
        }

     
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

golain_err_t golain_hal_mqtt_init(golain_t* golain){

    golain_err_t err = GOLAIN_OK;
    
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = "dev.golain.io",
        .port = 8083,
        .client_cert_pem = (const char*)golain->device_cert,
        .client_key_pem = (const char*)golain->device_pvt_key,
        .client_id = client_id,
    };

    esp_err_t res = esp_tls_init_global_ca_store();

    res = esp_tls_set_global_ca_store((unsigned char *)mqtt_root_ca_cert_pem_start, mqtt_root_ca_cert_pem_end-mqtt_root_ca_cert_pem_start); 

    if(res != 0){
        ESP_LOGE(TAG,"Error code: 0x%08x\n", res);
        return GOLAIN_ERR;
    }

    ESP_LOGD(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

    _golain_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(_golain_mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(_golain_mqtt_client);

    return err;
}


golain_err_t postShadow(uint8_t * data, int length){
    
    int post_err = esp_mqtt_client_publish(_golain_mqtt_client, DEVICE_SHADOW_TOPIC_U, (char*)data, length, 0, 1);

    if(post_err > 0){
    ESP_LOGE(TAG, "Error publishing to %s  Returned: %d", DEVICE_SHADOW_TOPIC_U, post_err);
    }
    return (golain_err_t) post_err;

}




void postData(char * data, size_t length, char * topic){ //Post already encoded data point
    char topic_to_publish[sizeof(DEVICE_DATA_TOPIC)+sizeof(topic)];
    sprintf(topic_to_publish, "%s/%s", DEVICE_DATA_TOPIC, topic);
    esp_mqtt_client_publish(_golain_mqtt_client, topic_to_publish, data, length, 0, 0);
    ESP_LOGI(TAG, "Published to topic: %s", topic);
}




golain_err_t postDeviceDataPoint(char* struct_name, const pb_msgdesc_t* descriptor, void * data, uint32_t length){
    esp_err_t err;    
    char topic_to_publish[sizeof(DEVICE_DATA_TOPIC)+strlen(struct_name)];
    sprintf(topic_to_publish, "%s/%s", DEVICE_DATA_TOPIC, struct_name);

    ESP_LOGI(TAG, "Topic: %s", topic_to_publish);
    uint8_t buffer[length];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, descriptor, data);
    if(!status){
        ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
        err = ESP_FAIL;
        return (golain_err_t) err;
    }
    ESP_LOGI(TAG, "Encoded %d bytes", stream.bytes_written);
    err = esp_mqtt_client_publish(_golain_mqtt_client, topic_to_publish, (char*)buffer, stream.bytes_written, 0, 0);
    return (golain_err_t)err;
}

golain_err_t postUserAssoc(void * UAData, size_t len){
    esp_err_t err;
    ESP_LOGW(TAG, "Publishing : %.*s", len, (char*)UAData);
    err = esp_mqtt_client_publish(_golain_mqtt_client, USER_ASSOC_TOPIC, (char*)UAData, len, 0, 0);
    return (golain_err_t)err;
}



uint8_t checkDataEvent(void){
    if(dataRcvFlag == 1){
        dataRcvFlag = 0;
        return 1;
    }
    return 0;
}


void getTopic( char * dest_topic_buffer, uint8_t * topiclen){
    memcpy(dest_topic_buffer, dirtyTopicArray, rcvTopicLength);
    memcpy(topiclen, &rcvTopicLength, sizeof(uint8_t));
}


void getData( char * dest_data_buffer, uint8_t * datalen){
    memcpy(dest_data_buffer, dirtyDataArray, rcvDataLength);
    memcpy(datalen, &rcvDataLength, sizeof(uint8_t));
}