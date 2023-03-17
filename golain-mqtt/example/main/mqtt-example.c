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

// void Demo_Task(void){
        
        
       
// }

cell Cell = cell_init_default;

uint8_t protobuff[cell_size];

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

    //xTaskCreate(Demo_Task, "Demo_Task", 4096, NULL, 10, &myTaskHandle);
    while(1){
        uint32_t timimg = 0;
        
        if(checkDataEvent()){
            getTopic(blankArray, &rcvtpclen);
            getData(blankArray2, &rcvdatalen);
            ESP_LOGI(TAG, "Received Topic: %.*s\r\n",rcvtpclen , blankArray);
            ESP_LOGI(TAG, "Received Topic: %.*s\r\n", rcvdatalen, blankArray2);
        }
        else{
            Cell.voltage = 3.33;
            Cell.temperature = (float)rand()*0.95;
            pb_ostream_t  ostream = pb_ostream_from_buffer(protobuff, sizeof(protobuff));
            pb_encode( &ostream, cell_fields, &Cell);
            postToDDTopic(protobuff, ostream.bytes_written);
            
            vTaskDelay(1500/ portTICK_RATE_MS);

            postDeviceDataPoint("TestPoint", cell_fields, &Cell, cell_size);            

            

        }
        vTaskDelay(1500/ portTICK_RATE_MS);
        
    }
 
}

