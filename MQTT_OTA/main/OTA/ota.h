#pragma once

#include <esp_ota_ops.h>
#include <mqtt_client.h>
#define OTA_TAG "OTA_TAG"

int current_len = 0;
uint8_t status = 0;
esp_ota_handle_t ota_handle = 0;

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