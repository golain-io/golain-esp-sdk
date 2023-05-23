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

#include "persistent_logs.h"

#define reset_counter_key   CONFIG_RESET_COUNTER_KEY
#define deviceHealthKey     CONFIG_DEVICE_HEALTH_STORE
#define protobufSize        CONFIG_DEVICE_HEALTH_BUFFER_SIZE
#define STORAGE_NAMESPACE   CONFIG_NVS_STORE_KEY

int numberOfResets = 0;

uint8_t buffer[protobufSize];
esp_chip_info_t info;