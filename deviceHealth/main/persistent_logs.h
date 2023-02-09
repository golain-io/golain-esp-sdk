#pragma once 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "driver/gpio.h"
#include "esp_log.h"



int errorCountSinceLastReset = 0;

#define P_LOG_I(tag, format, ...)                           \
  ESP_LOGI(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_INFO, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#define P_LOG_W(tag, format, ...)                           \
  ESP_LOGW(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  write_p_log(ESP_LOG_WARN, __func__, "[%s]: " format, tag, ##__VA_ARGS__);
#define P_LOG_E(tag, format, ...)                           \
  ESP_LOGE(tag, "(%s)-> " format, __func__, ##__VA_ARGS__); \
  /*write_p_log(ESP_LOG_ERROR, __func__, "[%s]: " format, tag, ##__VA_ARGS__);*/ \
  errorCountSinceLastReset++;