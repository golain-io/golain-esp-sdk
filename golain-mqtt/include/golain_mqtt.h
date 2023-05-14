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
//#include "esp_ota_ops.h"
#include <sys/param.h>




/// @brief Initialise and start MQTT service. 
/// @note If wifi is not initialised, this will reset the ESP. Change DHCP Length from 68 to 255 to void heap problems
void mqtt_app_start();

/// @brief Post data to /device-shadow/r topic
/// @param data This should have the buffer to the encoded shadow buffer
/// @param length Number of bytes written to the shadow buffer
/// @note By default shadow will be posted automatically when requested via {DEVICE_SHADOW_TOPIC}/r topic
/// @return 0 when no problems occur
esp_err_t postShadow(uint8_t * data, int length);


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
esp_err_t postDeviceDataPoint(char* struct_name, const pb_msgdesc_t* descriptor, void * data, uint32_t length);

/// @brief Post data to user association topic on the golain platform
/// @param UAData User association data
/// @param len Length of the data to be sent
/// @returns the error code in the form of esp_err_t. 0 on success
esp_err_t postUserAssoc(void * UAData, size_t len);

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