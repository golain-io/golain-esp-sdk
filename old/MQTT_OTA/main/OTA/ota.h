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

#pragma once

#include <esp_ota_ops.h>
#include <mqtt_client.h>
#include "../config/config.h"




esp_err_t ota_update_handler(esp_mqtt_event_handle_t event)
{
    current_len = current_len + event->data_len;
    ESP_LOGI(OTA_TAG, "current length %d  total length %d", current_len, event->total_data_len);

    // Check if the message is an OTA update payload

    // Get the OTA partition info
    const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL);
    if (partition == NULL)
    {
        ESP_LOGI(OTA_TAG, "Error: could not find a valid OTA partition");
        // Error: could not find a valid OTA partition
        return ESP_FAIL;
    }
    if (!status)
    {
        // Start the OTA update process
        if (esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &ota_handle) != ESP_OK)
        {
            // Error: could not start OTA update
            ESP_LOGI(OTA_TAG, "Error: could not start OTA update");
            return ESP_FAIL;
        }
        status = 1;
    }

    // Write the OTA update payload to the OTA partition
    const int len = event->data_len;
    esp_err_t writeError = esp_ota_write(ota_handle, (void *)event->data, len);
    if (writeError != ESP_OK)
    {
        ESP_LOGI(OTA_TAG, "Error: could not write OTA update payload to partition");
        ESP_LOGI(OTA_TAG, "%s", esp_err_to_name(writeError));
        return ESP_FAIL;
    }
    if (event->total_data_len == current_len)
    {
        ESP_LOGI(OTA_TAG, "Time to finalize the OTA ");
        // Finalize the OTA update
        if (esp_ota_end(ota_handle) != ESP_OK)
        {
            // Error: could not finalize OTA update
            return ESP_FAIL;
        }
        esp_err_t err = esp_ota_set_boot_partition(partition);
        // Reboot the device to activate the new firmware
        esp_restart();
        return ESP_OK;
    }

    return ESP_OK;
}