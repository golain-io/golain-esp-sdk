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
nvs_handle_t my_handle;

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
    //Configuration 
    custom_cb = temp_cfg.shadow_update_cb;
    //clean_nvs = temp_cfg.clean_on_error;
    
    
    //NVS 
    
    esp_err_t err = nvs_open("Shadow Buffer", NVS_READWRITE, &my_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised. HELP");
        return NVS_NOT_INIT;
    } 

    size = sizeof(shadow_buffer);
    
    err = nvs_get_blob(my_handle, "Shadow Buffer", shadow_buffer, &size);
    
    nvs_close(my_handle);
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

    nvs_close(my_handle);

    return GOLAIN_OK;
}


//-------------------------------------------------------------------------------------------------------------------------------------
enum golain_err_t updateNVS(uint8_t * buff, size_t len){
    enum golain_err_t shadow_err;
    esp_err_t err = nvs_open("Shadow Buffer", NVS_READWRITE, &my_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised. HELP");
        return NVS_NOT_INIT;
    } 

    err = nvs_set_blob(my_handle, "Shadow Buffer", buff, len);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be updated");
        
        //nvs_close(my_handle);
        shadow_err = NVS_UPDATE_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Updated");
        
        shadow_err = GOLAIN_OK;
    }

    nvs_commit(my_handle);
    nvs_close(my_handle);
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