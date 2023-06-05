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

//Library includes
#include "shadow_helper.h"
#include "shadow.pb.h"


//Protobuff headers
#include "pb_encode.c"
#include "pb_decode.c"
#include "pb_common.c"
#include "pb.h"

//ESP32 headers
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

//Defines
#define TAG "SHADOW HELPER"

//----------------------------------------------------------------------------------Global Variables start

//Protobuff
uint8_t shadow_buffer[128];
size_t size;
shadow Shadow = shadow_init_default;

//Configuration
void(* custom_cb)(void);
    //bool clean_nvs;

//NVS
nvs_handle_t shadow_nvs_handle;

//-----------------------------------------------------------------------------------------Functions start

golain_err_t UpdatewithStruct(void){
    ESP_LOGI(TAG, "Updated using struct");
    pb_ostream_t ostream = pb_ostream_from_buffer(shadow_buffer, shadow_size);
    
    if(!pb_encode(&ostream, shadow_fields, &Shadow)){
        ESP_LOGE(TAG,"%s",ostream.errmsg);
        return PB_ENCODE_FAIL;
    }
    updateNVS(shadow_buffer, shadow_size);
    custom_cb();
    return GOLAIN_OK;
}

//-----------------------------------------------------------------------------------------
golain_err_t UpdatewithBuff(uint8_t * buff, size_t len){

golain_err_t shadow_err;
if(buff != NULL){    
    pb_istream_t istream = pb_istream_from_buffer(buff, len);
    bool decode_status = pb_decode(&istream, shadow_fields, &Shadow);
    if(!decode_status){
        ESP_LOGE(TAG,"%s",istream.errmsg);
        ESP_LOGE(TAG, "Decoding failed");
        shadow_err = PB_UPDATE_FAIL;
    }
    else{
    updateNVS(buff,len);
    custom_cb();
    shadow_err = GOLAIN_OK;
    }
}
else{
    ESP_LOGI(TAG, "NULL Buffer");
    shadow_err = PB_UPDATE_FAIL;
}   
    
return shadow_err;
}

//---------------------------------------------------------------------------------------------------------------------------------
enum golain_err_t InitDeviceShadow(ShadowCfg temp_cfg){
 
    custom_cb = temp_cfg.shadow_update_cb;
    esp_err_t err = nvs_open(NVS_SHADOW_KEY, NVS_READWRITE, &shadow_nvs_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised. HELP");
        return NVS_NOT_INIT;
    } 

    size = sizeof(shadow_buffer);
    
    err = nvs_get_blob(shadow_nvs_handle, NVS_SHADOW_KEY, shadow_buffer, &size);
    
    nvs_close(shadow_nvs_handle);
    switch(err){
        
        case(ESP_ERR_NVS_NOT_INITIALIZED):
        ESP_LOGW(TAG, "NVS Not found");
        return NVS_NOT_INIT;
        
        
        case(ESP_OK): //Decode buffer into pb
        ESP_LOGD(TAG, "Buffer in NVS found");
        pb_istream_t istream = pb_istream_from_buffer(shadow_buffer, shadow_size);
        bool decode_state = pb_decode(&istream, shadow_fields, &Shadow);
        
        if(!decode_state){
            ESP_LOGE(TAG,"%s",istream.errmsg);
            ESP_LOGW(TAG, "Decoding failed");
            
            if(temp_cfg.clean_on_error){
                //Clean NVS section
                ESP_LOGW(TAG, "NVS Cleaned");
                //nvs_flash_erase();
                return ESP_OK;
            }
            return GENERIC_ERR;
        }

        return GOLAIN_OK;

        case(ESP_FAIL):
        ESP_LOGE(TAG, "Panicked");

        return GENERIC_ERR;

        default:

        break;
    }    

    nvs_close(shadow_nvs_handle);

    return GOLAIN_OK;
}


//-------------------------------------------------------------------------------------------------------------------------------------
enum golain_err_t updateNVS(uint8_t * buff, size_t len){
    enum golain_err_t shadow_err;
    esp_err_t err = nvs_open(NVS_SHADOW_KEY, NVS_READWRITE, &shadow_nvs_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised. HELP");
        return NVS_NOT_INIT;
    } 

    err = nvs_set_blob(shadow_nvs_handle, NVS_SHADOW_KEY, buff, len);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be updated");
        
        //nvs_close(shadow_nvs_handle);
        shadow_err = NVS_UPDATE_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Updated");
        
        shadow_err = GOLAIN_OK;
    }

    nvs_commit(shadow_nvs_handle);
    nvs_close(shadow_nvs_handle);
    return shadow_err;
}

//--------------------------------------------------------------------------------------------------------------------------------------
golain_err_t GetShadow(uint8_t * buff, size_t buff_len, size_t* encoded_size){
    pb_ostream_t ostream = pb_ostream_from_buffer(buff, shadow_size);
    if(!pb_encode(&ostream, shadow_fields, &Shadow)){
        ESP_LOGE(TAG,"%s",ostream.errmsg);
        return PB_ENCODE_FAIL;
    }
    *encoded_size = ostream.bytes_written;
    ESP_LOGI(TAG, "Encoded Length: %d", *encoded_size);
    return GOLAIN_OK;
}

//------------------------------------------------------------------------------------------------------------------------------------End