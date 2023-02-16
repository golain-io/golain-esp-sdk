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
#include "protobufs/deviceHealth.pb.h"
#include "protobufs/encoding_callbacks.h"
#include "protobufs/decoding_callbacks.h"
#define reset_counter_key  "qwerty"
#define deviceHealthKey  "kedskh"
#define protobufSize 256
uint8_t buffer[protobufSize];
#define STORAGE_NAMESPACE "storage"
int numberOfResets = 0;



#include "persistent_logs.h"

esp_chip_info_t info;