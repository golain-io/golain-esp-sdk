#include "golain.h"
#include "golain_hal.h"
#include "golain_constants.h"
#include "esp_log.h"

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#include <stdio.h>

#define TAG "GOLAIN"

/*------------------------------------------Variables-----------------------------------*/
void * _shadow_pointer;
pb_msgdesc_t* _shadow_fields;
size_t _shadow_size;


/*-----------------------------------------Call Back functions------------------------------------------*/
void (*_golain_mqtt_shadow_cb)(void);

/*-----------------------------------------User Level Functions-----------------------------------------*/
golain_err_t golain_init(golain_t *golain, golain_config_t *config) {
    golain_err_t err = GOLAIN_OK;

    if (config == NULL || golain == NULL) {
       return GOLAIN_ERR_INVALID_ARG;
        
    }
    
    
    // set config to golain struct
    golain->config = config;

    golain_hal_init(golain);

    #ifdef CONFIG_GOLAIN_BLE
    golain->ble = NULL;
    #endif

    #ifdef CONFIG_GOLAIN_WIFI
    // initialise wifi
    _golain_hal_wifi_init();
    #endif

    // initialise mqtt client
    err = _golain_hal_mqtt_init(golain);
    if (err != GOLAIN_OK) {
        return err;
    }
    
    err = _golain_hal_mqtt_connect();
    if (err != GOLAIN_OK) {
        return err;
    }

    #ifdef CONFIG_GOLAIN_BLE
    // initialise ble client
    err = golain_hal_ble_init(golain);
    if (err != GOLAIN_OK) {
        return err;
    }
    #endif

    return err;
}


/*-----------------------------------------MQTT---------------------------------------------------------*/
golain_err_t golain_mqtt_process_message(golain_t* _golain, char* topic, size_t topic_len, char * data, size_t data_len){
    golain_err_t err = GOLAIN_OK;
    
    if (topic == NULL || data == NULL) {
        return GOLAIN_ERR_INVALID_ARG;
    }
    
    // check if topic is shadow read
    if (strncmp(GOLAIN_SHADOW_READ_TOPIC, topic, GOLAIN_SHADOW_READ_TOPIC_LEN) == 0) {
        size_t _cpy_len = data_len > CONFIG_GOLAIN_SHADOW_BUFFER_SIZE ? CONFIG_GOLAIN_SHADOW_BUFFER_SIZE : data_len;
        memset(shadow_buffer, 0x00, CONFIG_GOLAIN_SHADOW_BUFFER_SIZE);
        memcpy(shadow_buffer, data, _cpy_len);
        _golain_shadow_update_from_buffer(_golain, shadow_buffer, _cpy_len);
    }
    
    return err;
}

golain_err_t golain_mqtt_post_data_point(char* topic, const void* descriptor, void * data, uint32_t length){
    golain_err_t err;
    
    uint8_t buffer[length];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, (pb_msgdesc_t * )descriptor, data);
    
    if(!status){
        ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
        err = GOLAIN_FAIL;
        return err;
    }
    
    ESP_LOGD(TAG, "Encoded %d bytes", stream.bytes_written);
    
    err = _golain_hal_mqtt_publish(topic, (char*)buffer, stream.bytes_written, 0, 0);
    
    if(err != 0){
        return GOLAIN_MQTT_PUBLISH_FAIL;
    }
    
    return GOLAIN_OK;
}


golain_err_t golain_mqtt_post_shadow(golain_t* _golain){
    int post_err = _golain_hal_mqtt_publish(GOLAIN_SHADOW_UPDATE_TOPIC, (char*)shadow_buffer, _golain->config->shadow_size, 1, 0);
    if(post_err > 0){
    
     return GOLAIN_MQTT_PUBLISH_FAIL;
    }
    return GOLAIN_OK;
}


/*-------------------------------------------------Device Shadow funcitons--------------------------*/
golain_err_t golain_shadow_init(golain_t* _golain){
//    custom_cb = temp_cfg.shadow_update_cb;
    golain_err_t nvs_err;
    size_t size = CONFIG_GOLAIN_SHADOW_BUFFER_SIZE;
    _shadow_fields = (pb_msgdesc_t *)(_golain->config->shadow_fields);
    _shadow_pointer = _golain->config->shadow_struct;
    _shadow_size = _golain->config->shadow_size;
    nvs_err = _golain_hal_shadow_persistent_read(shadow_buffer, size);
    if(!nvs_err){
        ESP_LOGD(TAG, "Buffer in NVS found");
        pb_istream_t istream = pb_istream_from_buffer(shadow_buffer, _shadow_size);
        bool decode_state = pb_decode(&istream, _shadow_fields, _shadow_pointer);
        
        if(!decode_state){
            ESP_LOGE(TAG,"%s",istream.errmsg);
            ESP_LOGW(TAG, "Decoding failed");
            return GOLAIN_FAIL;
        }
    }
    else{
     return NVS_READ_FAIL;   
    }
    return GOLAIN_OK;
}


golain_err_t _golain_shadow_update_from_buffer(golain_t* _golain, uint8_t * buff, size_t len){
    golain_err_t shadow_err = GOLAIN_OK;
    if(buff != NULL){    
        pb_istream_t istream = pb_istream_from_buffer(buff, len);
        bool decode_status = pb_decode(&istream, _golain->config->shadow_fields, _golain->config->shadow_struct);
        if(!decode_status){
            ESP_LOGE(TAG,"%s",istream.errmsg);
            ESP_LOGE(TAG, "Decoding failed");
            shadow_err = PB_UPDATE_FAIL;
            return shadow_err;
        }
        else{
            _golain_hal_shadow_persistent_write(buff, len);
            if (_golain->config->on_shadow_update_from_mqtt != NULL){
                _golain->config->on_shadow_update_from_mqtt();
            }
        }
    }
    else{
        ESP_LOGI(TAG, "NULL Buffer");
        shadow_err = PB_UPDATE_FAIL;
    }   
    return shadow_err;
}

golain_err_t golain_shadow_update(golain_t *golain){
    ESP_LOGD(TAG, "Updated using struct");
    pb_ostream_t ostream = pb_ostream_from_buffer(shadow_buffer, _shadow_size);
    
    if(!pb_encode(&ostream, _shadow_fields, _shadow_pointer)){
        ESP_LOGE("TAG","%s",ostream.errmsg);
        return PB_ENCODE_FAIL;
    }
    // update in local store    
    _golain_hal_shadow_persistent_write(shadow_buffer, _shadow_size);
    // publish to cloud
    _golain_hal_mqtt_publish(GOLAIN_SHADOW_UPDATE_TOPIC, (char*)shadow_buffer, _shadow_size, 1, 0);
    
    return GOLAIN_OK;
}