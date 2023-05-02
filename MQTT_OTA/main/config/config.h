#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include <esp_ota_ops.h>
#include <mqtt_client.h>

//Tags
#define WIFI_TAG "WIFI_TAG"
#define MQTT_TAG "MQTT TAG"
#define OTA_TAG "OTA_TAG"

//MQTT params
#define MQTT_URI "mqtt://broker.hivemq.com"
#define MQTT_PORT 1883


//OTA params
int current_len = 0;
uint8_t status = 0;
esp_ota_handle_t ota_handle = 0;

//Wifi Params
#define CONFIG_WIFI_SSID "FLBS"
#define CONFIG_WIFI_PASSWORD "hellowworld"
static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;