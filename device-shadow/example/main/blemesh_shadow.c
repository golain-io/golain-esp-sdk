/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"
/* BLE */
#include "golain_blemesh_comp.h"

/*Shadow includes*/
#include "shadow_helper.h"
#include "shadow.pb.h"

extern shadow Shadow;

uint8_t myString[] = {12,34,56,78,90};

uint8_t otherString[] = {10,29,38,48,57,66};

uint8_t errrr[] = {1,2,3,4,5,6,7,8,9};

//-----------------------------------------------------------------------------------------------------------------Callback functions
static void example_control_get_cb(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf){
    ESP_LOGW("EXAMPLE", "CONTROL GET TRIGGERED");
    uint8_t tempbuff[shadow_size];
    size_t encoded_size;
    GetShadow(tempbuff, shadow_size, &encoded_size);
    ESP_LOGI("EXAMPLE", "Encoded Shadow");
    send_message(model, ctx, buf, tempbuff, encoded_size);
    

}

static void example_control_set_cb(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf){
    
    char Reply[] = "Rcvd"; 
    
    ESP_LOGW("EXAMPLE", "CONTROL SET TRIGGERED");
    //uint8_t temp[128];
    //memcpy(temp,buf->om_data,buf->om_len);
    UpdatewithBuff(buf->om_data,buf->om_len);
    send_message(model, ctx, buf, (uint8_t*)Reply, sizeof(Reply));

}

static void example_data_get_cb(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf){
    ESP_LOGW("EXAMPLE", "DATA GET TRIGGERED");
    uint8_t tempbuff[buf->om_len];
    memcpy(tempbuff,buf->om_data,buf->om_len);

    switch(tempbuff[0]){
        case 't':
        send_message(model, ctx, buf, myString, sizeof(myString));
        break;
        case 'r':
        send_message(model,ctx,buf,otherString, sizeof(otherString));
        break;
        default:
        
        send_message(model,ctx,buf, errrr, sizeof(errrr));
        break;
    }
}

static void ShadowCallback(void){
    ESP_LOGI("MAIN", "Shadow updated %d %d %d", Shadow.r, Shadow.g, Shadow.b);
}


//  ---------------------------------------------------------------------------------------------------------------Config definitions 
MeshConfig myConfig ={
  .user_control_get_cb = example_control_get_cb,
  .user_control_set_cb = example_control_set_cb,
  .user_data_get_cb = example_data_get_cb,
};

ShadowCfg main_config ={
    .clean_on_error = 0,
    .shadow_update_cb = ShadowCallback,
};

void app_main(void)
{

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    InitDeviceShadow(main_config);
    printf("\n Shadow on init: %d %d %d \n", Shadow.b, Shadow.r, Shadow.g);
    
    
    ESP_LOGI("MAIN", "Starting");
    bt_mesh_custom_start(myConfig);
}
