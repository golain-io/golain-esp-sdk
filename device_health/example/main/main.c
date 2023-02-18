
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include <pb.h>
#include "constants.h"
#include "persistent_logs.h"
#include "device_health.h"



void app_main(void)
{

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    for (int i = 0; i < 10; i++)
    {
        P_LOG_E("main", "This is not an error!");
    }

   
    size_t message_length;

    /* Encode our message */
    if (!encode_message(buffer, sizeof(buffer), &message_length))
    {
        return;
    }

    err  = deviceHealthStore(buffer);
    if (err == ESP_OK){
        ESP_LOGI("main","device health Stored successfully");
    }
    else{
    ESP_LOGI("main","health store %s",esp_err_to_name(err));
    }

    /* But because we are lazy, we will just decode it immediately. */
    decode_message(buffer, message_length);

}
