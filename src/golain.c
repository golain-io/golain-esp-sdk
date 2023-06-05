#include "golain.h"
#include "golain_hal.h"
#include "golain_constants.h"
// #include "esp_log.h"

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#include "device_health.pb.h"
#include "logs.pb.h"
#include "golain_constants.h"

#include <stdio.h>

#define TAG "GOLAIN"

/*------------------------------------------Variables-----------------------------------*/
void * _shadow_pointer;
pb_msgdesc_t* _shadow_fields;
size_t _shadow_size;
//Change to save

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

    golain_shadow_init(golain);

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
golain_err_t golain_mqtt_process_message(golain_t* _golain, char* topic, size_t topic_len, char* data, size_t data_len, size_t total_len) {
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

    #ifdef CONFIG_GOLAIN_MQTT_OTA
    
    if (strncmp(GOLAIN_OTA_TOPIC, topic, GOLAIN_OTA_UPDATE_TOPIC_LEN) == 0) {
        // check if full topic is firmware update
        if (strncmp(GOLAIN_OTA_UPDATE_TOPIC, topic, GOLAIN_OTA_UPDATE_TOPIC_LEN) == 0){
            // update firmware
            err = _golain_hal_ota_update(total_len, data, data_len);
        }
        else {
            // call user defined callback
            if (_golain->config->on_ota_payload_received != NULL) {
                _golain->config->on_ota_payload_received(data, data_len);
            }
        }
    }

    #endif
    
    return err;
}

golain_err_t golain_mqtt_post_data_point(char* topic, const void* descriptor, void * data, uint32_t length){
    golain_err_t err;
    
    uint8_t buffer[length];

    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, (pb_msgdesc_t * )descriptor, data);
    
    if(!status){
        GOLAIN_LOG_E(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
        err = GOLAIN_FAIL;
        return err;
    }
    
    GOLAIN_LOG_D(TAG, "Encoded %d bytes", stream.bytes_written);
    
    err = _golain_hal_mqtt_publish(topic, (char*)buffer, stream.bytes_written, 0, 0);
    
    if(err != 0){
        return GOLAIN_MQTT_PUBLISH_FAIL;
    }
    
    return GOLAIN_OK;
}


golain_err_t golain_mqtt_post_shadow(golain_t* _golain){
    size_t size_encoded;
    _golain_shadow_get_trimmed_shadow_buffer(_golain, &size_encoded);

    int ret_val = _golain_hal_mqtt_publish(GOLAIN_SHADOW_UPDATE_TOPIC, (char*)shadow_buffer, size_encoded, 1, 0);
    GOLAIN_LOG_D(TAG, "Returned msg ID: %d", ret_val);

    return GOLAIN_OK;
}

#ifdef CONFIG_GOLAIN_CONSUMER_ENABLED

golain_err_t golain_mqtt_post_user_assoc(golain_t* _golain, uint8_t * buff, uint8_t len){
    int post_err = _golain_hal_mqtt_publish(GOLAIN_USER_ASSOC_TOPIC, (char*)buff, len, 0, 0);
    if(post_err > 0){
        return GOLAIN_MQTT_PUBLISH_FAIL;
    }
    return GOLAIN_OK;
}

#endif

/*-------------------------------------------------Device Shadow funcitons--------------------------------------------------*/
golain_err_t golain_shadow_init(golain_t* _golain){
//    custom_cb = temp_cfg.shadow_update_cb;
    golain_err_t nvs_err;
    size_t size = CONFIG_GOLAIN_SHADOW_BUFFER_SIZE;
    _shadow_fields = (pb_msgdesc_t *)(_golain->config->shadow_fields);
    _shadow_pointer = _golain->config->shadow_struct;
    _shadow_size = _golain->config->shadow_size;
    nvs_err = _golain_hal_shadow_persistent_read(shadow_buffer, size);
    if(!nvs_err){
        GOLAIN_LOG_D(TAG, "Buffer in NVS found");
        pb_istream_t istream = pb_istream_from_buffer(shadow_buffer, _shadow_size);
        bool decode_state = pb_decode(&istream, _shadow_fields, _shadow_pointer);
        
        if(!decode_state){
            GOLAIN_LOG_E(TAG,"%s",istream.errmsg);
            GOLAIN_LOG_W(TAG, "Decoding failed");
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
            GOLAIN_LOG_E(TAG,"%s",istream.errmsg);
            GOLAIN_LOG_E(TAG, "Decoding failed");
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
        GOLAIN_LOG_E(TAG, "NULL Buffer");
        shadow_err = PB_UPDATE_FAIL;    
    }   
    return shadow_err;
}

golain_err_t golain_shadow_update(golain_t *golain){
    GOLAIN_LOG_D(TAG, "Updated using struct");
    pb_ostream_t ostream = pb_ostream_from_buffer(shadow_buffer, _shadow_size);
    
    if(!pb_encode(&ostream, _shadow_fields, _shadow_pointer)){
        GOLAIN_LOG_E("TAG","%s",ostream.errmsg);
        return PB_ENCODE_FAIL;
    }
    // update in local store    
    _golain_hal_shadow_persistent_write(shadow_buffer, _shadow_size);
    // publish to cloud
    _golain_hal_mqtt_publish(GOLAIN_SHADOW_UPDATE_TOPIC, (char*)shadow_buffer, _shadow_size, 1, 0);
    
    return GOLAIN_OK;
}

golain_err_t _golain_shadow_get_trimmed_shadow_buffer(golain_t * golain, size_t* encoded_size){
    pb_ostream_t ostream = pb_ostream_from_buffer(shadow_buffer, golain->config->shadow_size);
    if(!pb_encode(&ostream, golain->config->shadow_fields, golain->config->shadow_struct)){
        GOLAIN_LOG_E(TAG,"%s",ostream.errmsg);
        return PB_ENCODE_FAIL;
    }
    *encoded_size = ostream.bytes_written;
    pb_get_encoded_size(encoded_size, golain->config->shadow_fields, golain->config->shadow_struct);
    GOLAIN_LOG_I(TAG, "Encoded Length: %d", *encoded_size);
    return GOLAIN_OK;

}

/*----------------------------------------------------------Device Health-------------------------------------------------------------------*/
#ifdef CONFIG_GOLAIN_DEVICE_HEALTH

golain_err_t golain_device_health_encode_message(uint8_t *buffer, size_t buffer_size, size_t *message_length){
    char data[] = "Zephyr is better\n";
    bool status;
    esp_chip_info(&info);

    /* Allocate space on the stack to store the message data.
     *
     * Nanopb generates simple struct definitions for all the messages.
     * - check out the contents of simple.pb.h!
     * It is a good idea to always initialize your structures
     * so that you do not have garbage data from RAM in there.
     */
    deviceHealth message = deviceHealth_init_zero;

    /* Create a stream that will write to our buffer. */
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, buffer_size);
    message.lastRebootReason = esp_reset_reason();
    message.deviceRevision = info.revision;
    message.numberOferrorsSinceLastReboot = errorCountSinceLastReset;
    message.numberOfReboots = restart_counter();
    message.userStringData.funcs.encode = golain_pb_encode_string;
    message.userStringData.arg = data;

    /* Now we are ready to encode the message! */
    status = pb_encode(&stream, deviceHealth_fields, &message);
    *message_length = stream.bytes_written;

    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        return PB_ENCODE_FAIL;
    }

    return GOLAIN_OK;

}

golain_err_t golain_device_health_decode_message(uint8_t *buffer, size_t message_length){
     bool status;
    char rx[20];
    /* Allocate space for the decoded message. */
    deviceHealth message = deviceHealth_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    message.userStringData.funcs.decode = golain_pb_decode_string;
    message.userStringData.arg = rx;

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, deviceHealth_fields, &message);

    /* Check for errors... */
    if (status)
    {

        GOLAIN_LOG_I("debug", "was called from %s", __func__);
        GOLAIN_LOG_I("decode", "number of errors since last reboot: %d", message.numberOferrorsSinceLastReboot);
        GOLAIN_LOG_I("decode", "last reboot reason %d", message.lastRebootReason);
        GOLAIN_LOG_I("decode", "number of reboots: %d", message.numberOfReboots);
        GOLAIN_LOG_I("decode", "chip revision: %d", message.deviceRevision);
        GOLAIN_LOG_I("decode", "user numeric data: %f", message.userNumericData);
        GOLAIN_LOG_I("decode", "user string data: %s", rx);
    }
    else
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    }

    return status;
}

bool golain_pb_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg){
    uint8_t buffer[1024] = {0};
    
    /* We could read block-by-block to avoid the large buffer... */
    if (stream->bytes_left > sizeof(buffer) - 1)
        return false;
    
    if (!pb_read(stream, buffer, stream->bytes_left))
        return false;
    
    sprintf((char*)*arg, "%s", buffer);
    return true;
    
}

bool golain_pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg){
    char* string_to_encode = (char*)*arg;
    if (!pb_encode_tag_for_field(stream, field))
        return false;
    
    return pb_encode_string(stream, (uint8_t*)string_to_encode, strlen(string_to_encode));
}
#endif