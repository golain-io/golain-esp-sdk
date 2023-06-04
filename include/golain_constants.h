#ifndef _GOLAIN_CONSTANTS_H
#define _GOLAIN_CONSTANTS_H

#define GOLAIN_VERSION "0.1.0b"


#define NVS_SHADOW_KEY "golain_shadow"

// check if the root topic and device name have been set
#ifndef CONFIG_GOLAIN_ROOT_TOPIC
    #warning "CONFIG_GOLAIN_ROOT_TOPIC not set. Using default value of 'golain'"
#endif

#ifndef CONFIG_GOLAIN_DEVICE_NAME
    #warning "CONFIG_GOLAIN_DEVICE_NAME not set. Using default value of 'esp32'"
#endif

#define GOLAIN_MQTT_BROKER_URI "mqtts://dev.golain.io"
#define GOLAIN_MQTT_BROKER_PORT 8083

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