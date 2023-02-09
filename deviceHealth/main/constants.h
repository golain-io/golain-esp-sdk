#pragma once 

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "nanopb/pb.h"
#include "nanopb/deviceHealth.pb.h"
#include "protobufs/encoding_callbacks.h"
#include "protobufs/decoding_callbacks.h"
const char *reset_counter_key = "qwerty";
const char *deviceHealthKey = "kedskh";
int numberOfResets = 0;
#define protobufSize 256

#define STORAGE_NAMESPACE "storage"
#include "persistent_logs.h"

esp_chip_info_t info;