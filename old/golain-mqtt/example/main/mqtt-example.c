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

/* MQTT over SSL Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

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
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

//#include "persistent_logs.h"
#include "golain.h"

#include "cell_data.pb.h"

#define TAG "MQTTS_EXAMPLE"



char blankArray[96];

        char blankArray2[128];
        uint8_t perlog_buffer[256];

        uint8_t rcvdatalen = 0;
        uint8_t rcvtpclen = 0; 



cell Cell = cell_init_default;

uint8_t protobuff[cell_size];

TaskHandle_t myTaskHandle;


void ShadowTask(void){
    P_LOG_I(TAG, "Shadow updated");
}



ShadowCfg myConfig= {
.shadow_update_cb = ShadowTask,
.clean_on_error = 0

};

golain_config Gconfig = {
    .root_topic = "/6969_6969"
    .client_id = "Test_ESP",
    .shadowcfg = myConfig,
}

void Demo_Task(void){
        while(1){
            Cell.voltage = ((rand()%(2200-200+1)) +200)*0.1;
            Cell.demo_test_int = (int32_t)rand();
            
            golain_post_device_data_point("Test_1", cell_fields, &Cell, cell_size);

            //postDeviceDataPoint("Demo-DP", cell_fields, &Cell, cell_size);            
            
            vTaskDelay(1000/ portTICK_RATE_MS);
        }
        
       
 }

void app_main(void)
{   
    
    
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    // Add nvs cleaning code
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    

    golain_init(&Gconfig);

    xTaskCreate(Demo_Task, "Demo_Task", 4096, NULL, 1, &myTaskHandle);
    
 
}

