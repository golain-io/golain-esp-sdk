#pragma once
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "pb.h"
#include "device_health.pb.h"
#include "encode_callbacks.h"
#include "decode_callbacks.h"
#include "constants.h"
#include "persistent_logs.h"

esp_err_t deviceHealthStore(uint8_t *deviceHealthproto)
{

    nvs_handle_t shadow_nvs_handle;

    // Open
    esp_err_t err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &shadow_nvs_handle);
    if (err != ESP_OK)
        return err;

    err = nvs_set_blob(shadow_nvs_handle, deviceHealthKey, deviceHealthproto, protobufSize);
    if (err != ESP_OK)
        return err;

    nvs_close(shadow_nvs_handle);
    return err;
}

int restart_counter() {
    int8_t num = 0;
    nvs_handle_t shadow_nvs_handle;
    esp_err_t err;

    // Open
    err = nvs_open(STORAGE_NAMESPACE, NVS_READWRITE, &shadow_nvs_handle);
    if (err != ESP_OK) {
      ESP_LOGE(LOGGING_TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }

    err = nvs_get_i8(shadow_nvs_handle, reset_counter_key, &num);
    if (err != ESP_OK) {
      ESP_LOGE(LOGGING_TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }

    num++;
    err = nvs_set_i8(shadow_nvs_handle, reset_counter_key, num);
    if (err != ESP_OK) {
      ESP_LOGE(LOGGING_TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }
   
    nvs_close(shadow_nvs_handle);
    
    return num;
}

bool encode_message(uint8_t *buffer, size_t buffer_size, size_t *message_length)
{
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
    message.userNumericData = 569.78;
    message.numberOfReboots = _golain_hal_reset_counter();
    message.userStringData.funcs.encode = encode_string;
    message.userStringData.arg = data;

    /* Now we are ready to encode the message! */
    status = pb_encode(&stream, deviceHealth_fields, &message);
    *message_length = stream.bytes_written;

    if (!status)
    {
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
    }

    return status;
}

bool decode_message(uint8_t *buffer, size_t message_length)
{
    bool status;
    char rx[20];
    /* Allocate space for the decoded message. */
    deviceHealth message = deviceHealth_init_zero;

    /* Create a stream that reads from the buffer. */
    pb_istream_t stream = pb_istream_from_buffer(buffer, message_length);

    message.userStringData.funcs.decode = decode_string;
    message.userStringData.arg = rx;

    /* Now we are ready to decode the message. */
    status = pb_decode(&stream, deviceHealth_fields, &message);

    /* Check for errors... */
    if (status)
    {

        ESP_LOGI("debug", "was called from %s", __func__);
        ESP_LOGI("decode", "number of errors since last reboot: %d", message.numberOferrorsSinceLastReboot);
        ESP_LOGI("decode", "last reboot reason %d", message.lastRebootReason);
        ESP_LOGI("decode", "number of reboots: %d", message.numberOfReboots);
        ESP_LOGI("decode", "chip revision: %d", message.deviceRevision);
        ESP_LOGI("decode", "user numeric data: %f", message.userNumericData);
        ESP_LOGI("decode", "user string data: %s", rx);
    }
    else
    {
        printf("Decoding failed: %s\n", PB_GET_ERROR(&stream));
    }

    return status;
}