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

#ifndef _GOLAIN_MQTT_H_
#define _GOLAIN_MQTT_H_


#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
#include "pb.h"

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "golain_constants.h"
#include "golain_err.h"
//#include "esp_ota_ops.h"
#include <sys/param.h>

#define DEVICE_SHADOW_TOPIC_R   GOLAIN_SHADOW_READ_TOPIC
#define DEVICE_SHADOW_TOPIC_U   GOLAIN_SHADOW_UPDATE_TOPIC
#define DEVICE_OTA_TOPIC        GOLAIN_OTA_TOPIC
#define DEVICE_DATA_TOPIC       GOLAIN_DATA_TOPIC
#define USER_ASSOC_TOPIC        GOLAIN_USER_ASSOC_TOPIC
#include "golain_err.h"
#define TAG "GOLAIN MQTT"


static uint8_t dataRcvFlag = 0;

char * split_topic[3]; 

extern Shadow shadow;

esp_mqtt_client_handle_t _golain_mqtt_client; 
static char dirtyTopicArray[96]; //We have a lot of memory screw it
static char dirtyDataArray[128]; //We have a lot of memory screw it

static uint8_t rcvTopicLength = 0;
static uint8_t rcvDataLength = 0;



extern const char mqtt_device_cert_pem_start[] asm("_binary_device_cert_pem_start");
extern const char mqtt_device_pvt_key_pem_start[] asm("_binary_device_private_key_pem_start");
extern const char mqtt_broker_cert_pem_start[] asm("_binary_mqtt_broker_cert_pem_start");
extern const unsigned char mqtt_root_ca_cert_pem_start[] asm("_binary_root_ca_cert_pem_start");

extern const char mqtt_device_cert_pem_end[] asm("_binary_device_cert_pem_end");
extern const char mqtt_device_pvt_key_pem_end[] asm("_binary_device_private_key_pem_end");
extern const char mqtt_broker_cert_pem_end[] asm("_binary_mqtt_broker_cert_pem_end");
extern const unsigned char mqtt_root_ca_cert_pem_end[] asm("_binary_root_ca_cert_pem_end");



/// @brief Initialise and start MQTT service. 
/// @note If wifi is not initialised, this will reset the ESP. Change DHCP Length from 68 to 255 to void heap problems
void mqtt_app_start(uint8_t* client_id,uint8_t* client_cert,uint8_t* client_key);

/// @brief Post data to /device-shadow/r topic
/// @param data This should have the buffer to the encoded shadow buffer
/// @param length Number of bytes written to the shadow buffer
/// @note By default shadow will be posted automatically when requested via {DEVICE_SHADOW_TOPIC}/r topic
/// @return 0 when no problems occur
golain_err_t postShadow(uint8_t * data, int length);


/// @brief A generic data posting function
/// @param data A byte array that contains the data to be sent
/// @param length Length of ```data``` array 
/// @param topic Character array or string of the topic where the data will be posted
void postData(char * data, size_t length, char * topic);


/// @brief Find a String in a string array
/// @param input_array String array to search in
/// @param array_len Length of ```input_array```
/// @param myTopic String to search in a ```input_array```
/// @return 
int8_t string_switch(char * input_array[], uint8_t array_len, char * myTopic);

/**
 * @brief Takes a struct, it's name, and the field name and posts the data to the respective topic
 * @param struct_name char* The name of the struct. The topic where data will be published will be ```{DEVICE_DATA_TOPIC}/<struct_name>```
 * @param descriptor The proto buffer fields. It should look like ```<struct_name>_fields```
 * @param data pointer to the protobuffer struct to be encoded and postedc
 * @param length Length of Device Data struct  
 * @returns the error code in the form of esp_err_t. 0 on success  
 */
golain_err_t postDeviceDataPoint(char* struct_name, const pb_msgdesc_t* descriptor, void * data, uint32_t length);

/// @brief Post data to user association topic on the golain platform
/// @param UAData User association data
/// @param len Length of the data to be sent
/// @returns the error code in the form of golain_err_t. 0 on success
golain_err_t postUserAssoc(void * UAData, size_t len);

/// @brief Check if a subscribed topic has data posted
///   
/// @return Returns 1 if data was received via mqtt and 0 if no data received 
uint8_t checkDataEvent(void);

/// @brief Get topic string along with the topic length
/// @param dest_topic_buffer buffer to hold the received topic
/// @param topiclen Length of received topic
void getTopic( char * dest_topic_buffer, uint8_t * topiclen);

/// @brief Get received data from any topic
/// @param dest_data_buffer buffer to hold the data received
/// @param datalen Length of the data received
void getData( char * dest_data_buffer, uint8_t * datalen);

#endif