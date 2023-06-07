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

#ifndef _GOLAIN_CONSTANTS_H
#define _GOLAIN_CONSTANTS_H

#define GOLAIN_VERSION "0.1.0b"

#include "sdkconfig.h"


#define GOLAIN_SHADOW_NVS_KEY                           "golain_shadow"
#define GOLAIN_DEVICE_HEALTH_NVS_KEY                    "golain_dev_health"
#define GOLAIN_DEVICE_HEALTH_RESET_COUNTER_NVS_KEY      "reset_counter"

// check if the root topic and device name have been set
#ifndef CONFIG_GOLAIN_ROOT_TOPIC
    #error "CONFIG_GOLAIN_ROOT_TOPIC not set."
#endif

#ifndef CONFIG_GOLAIN_DEVICE_NAME
    #error "CONFIG_GOLAIN_DEVICE_NAME not set."
#endif

#define GOLAIN_MQTT_BROKER_URI "mqtts://dev.golain.io"
#define GOLAIN_MQTT_BROKER_PORT 8083

#define GOLAIN_MQTT_BROKER_URL "mqtts://dev.golain.io:8083"

// MQTT topics for all actions
#define GOLAIN_SHADOW_TOPIC CONFIG_GOLAIN_ROOT_TOPIC CONFIG_GOLAIN_DEVICE_NAME "/device-shadow"
#define GOLAIN_SHADOW_UPDATE_TOPIC GOLAIN_SHADOW_TOPIC "/u"
#define GOLAIN_SHADOW_READ_TOPIC GOLAIN_SHADOW_TOPIC "/r"

// the suffix for the data topic is the same as the data point name defined on the platform
#define GOLAIN_DATA_TOPIC CONFIG_GOLAIN_ROOT_TOPIC CONFIG_GOLAIN_DEVICE_NAME "/device-data/"

// suffix for user association topic
#define GOLAIN_USER_ASSOC_TOPIC CONFIG_GOLAIN_ROOT_TOPIC CONFIG_GOLAIN_DEVICE_NAME "/user"


// suffixes for OTA topics are the same as defined in each artifact within a release
#define GOLAIN_OTA_TOPIC CONFIG_GOLAIN_ROOT_TOPIC CONFIG_GOLAIN_DEVICE_NAME "/ota/"
#define GOLAIN_OTA_UPDATE_TOPIC GOLAIN_OTA_TOPIC "firmware"
#define GOLAIN_OTA_STATUS_TOPIC GOLAIN_OTA_TOPIC "status"
#define GOLAIN_OTA_STATUS_SUCCESS "success"
#define GOLAIN_OTA_STATUS_FAILED "failure"

#define GOLAIN_LOG_TOPIC CONFIG_GOLAIN_ROOT_TOPIC CONFIG_GOLAIN_DEVICE_NAME "/device-logs"

// Topic Lengths
#define GOLAIN_SHADOW_UPDATE_TOPIC_LEN  ( ( uint16_t ) ( sizeof( GOLAIN_SHADOW_UPDATE_TOPIC ) - 1 ) )
#define GOLAIN_SHADOW_READ_TOPIC_LEN    ( ( uint16_t ) ( sizeof( GOLAIN_SHADOW_READ_TOPIC ) - 1 ) )
#define GOLAIN_DATA_TOPIC_LEN           ( ( uint16_t ) ( sizeof( GOLAIN_DATA_TOPIC ) - 1 ) )
#define GOLAIN_OTA_UPDATE_TOPIC_LEN     ( ( uint16_t ) ( sizeof( GOLAIN_OTA_UPDATE_TOPIC ) - 1 ) )
#define GOLAIN_OTA_STATUS_TOPIC_LEN     ( ( uint16_t ) ( sizeof( GOLAIN_OTA_STATUS_TOPIC ) - 1 ) )
#define GOLAIN_LOG_TOPIC_LEN            ( ( uint16_t ) ( sizeof( GOLAIN_LOG_TOPIC ) - 1 ) )

#endif