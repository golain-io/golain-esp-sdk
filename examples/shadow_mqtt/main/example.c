#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "golain.h"
#include "shadow.pb.h"

extern const char mqtt_device_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const char mqtt_device_cert_pem_end[] asm("_binary_device_cert_pem_end");

extern const char mqtt_device_key_pem_start[] asm("_binary_device_private_key_pem_start");
extern const char mqtt_device_key_pem_end[] asm("_binary_device_private_key_pem_end");

extern const char mqtt_ca_cert_pem_start[] asm("_binary_root_ca_cert_pem_start");
extern const char mqtt_ca_cert_pem_end[] asm("_binary_root_ca_cert_pem_end");

extern const char mqtt_broker_cert_pem_start[] asm("_binary_mqtt_broker_cert_pem_start");
extern const char mqtt_broker_cert_pem_end[] asm("_binary_mqtt_broker_cert_pem_end");

Shadow shadow = Shadow_init_zero;

void app_main() {

    nvs_flash_init();

    golain_config_t golain_config = {

        .shadow_struct = &shadow,
        .shadow_size = Shadow_size,
        .shadow_fields = Shadow_fields,

        .device_cert = mqtt_device_cert_pem_start,
        .device_pvt_key = mqtt_device_key_pem_start,
        .root_ca_cert_start = mqtt_ca_cert_pem_start,
        .root_ca_cert_end = mqtt_ca_cert_pem_end,
        .broker_cert = mqtt_ca_cert_pem_start,

        .wifi_ssid = "FLBS",
        .wifi_password = "hellowworld"

    };

    golain_t golain = {};

    golain_init(&golain, &golain_config);

    while (1) {
        vTaskDelay(1);
    }

}