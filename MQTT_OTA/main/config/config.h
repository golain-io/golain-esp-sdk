#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"
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


#include "driver/rmt.h"
#include "led_strip.h"


#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define EXAMPLE_CHASE_SPEED_MS (10)

#define WIFI_TAG "WIFI_TAG"


rmt_config_t rmt_config112;

uint32_t red=0,blue=0,green=0;

led_strip_config_t strip_config;

led_strip_t *strip;
#define EXAMPLE_CHASE_SPEED_MS (10)

static EventGroupHandle_t wifi_event_group;
const static int CONNECTED_BIT = BIT0;



/*
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/rmt.h"
#include "led_strip.h"

#define LED_STRIP_TAG = "LED_STRIP_TAG";

#define RMT_TX_CHANNEL RMT_CHANNEL_0

#define EXAMPLE_CHASE_SPEED_MS (10)

rmt_config_t rmt_config112;

uint32_t red,blue,green;

led_strip_config_t strip_config;

led_strip_t *strip;
#define EXAMPLE_CHASE_SPEED_MS (10)
*/