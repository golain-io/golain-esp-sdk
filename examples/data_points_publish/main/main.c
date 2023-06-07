/*
 * Copyright (c) 2023 Quoppo LLC
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

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "golain.h"
#include "shadow.pb.h"
#include "data.pb.h"

extern const char mqtt_device_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const char mqtt_device_cert_pem_end[] asm("_binary_device_cert_pem_end");

extern const char mqtt_device_key_pem_start[] asm("_binary_device_private_key_pem_start");
extern const char mqtt_device_key_pem_end[] asm("_binary_device_private_key_pem_end");

extern const char mqtt_ca_cert_pem_start[] asm("_binary_root_ca_cert_pem_start");
extern const char mqtt_ca_cert_pem_end[] asm("_binary_root_ca_cert_pem_end");

extern const char mqtt_broker_cert_pem_start[] asm("_binary_mqtt_broker_cert_pem_start");
extern const char mqtt_broker_cert_pem_end[] asm("_binary_mqtt_broker_cert_pem_end");

Shadow shadow = Shadow_init_zero;

MPU myMPU = MPU_init_zero;





void app_main() {

    nvs_flash_init();
    golain_config_t golain_config = {

        .shadow_struct = &shadow,
        .shadow_size = Shadow_size,
        .shadow_fields = Shadow_fields,

        .device_cert = mqtt_device_cert_pem_start,
        .device_pvt_key = mqtt_device_key_pem_start,
        .root_ca_cert_start = mqtt_ca_cert_pem_start,
        .root_ca_cert_len = (mqtt_ca_cert_pem_end-mqtt_ca_cert_pem_start),
    };
    golain_t golain = {};
    golain_init(&golain, &golain_config);

    while (1) {
        myMPU.ax = (int32_t)rand();
        myMPU.ay = (int32_t)rand();
        myMPU.az = (int32_t)rand();
        myMPU.gx = (int32_t)rand();
        myMPU.gy = (int32_t)rand();
        myMPU.gz = (int32_t)rand();


        golain_mqtt_post_data_point(DATA_TOPIC("MPU"), MPU_fields, &myMPU, MPU_size);
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

}