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
#include "golain_mqtt.h"
#include "mk_wifi_helper.h"
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

void Demo_Task(void){
        while(1){
            Cell.voltage = ((rand()%(2200-200+1)) +200)*0.1;
            Cell.demo_test_int = (int32_t)rand();
            pb_ostream_t  ostream = pb_ostream_from_buffer(protobuff, sizeof(protobuff));
            pb_encode( &ostream, cell_fields, &Cell);
            
            postDeviceDataPoint("Demo-DP", cell_fields, &Cell, cell_size);            
            
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
    

    wifi_init_sta(); 
    //ESP_LOGI(TAG, "wifi_init_sta finished.");

    mqtt_app_start();

    xTaskCreate(Demo_Task, "Demo_Task", 4096, NULL, 1, &myTaskHandle);
    
 
}

