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





void mqtt_app_start();

esp_err_t postToDDTopic(uint8_t * data, int length);

void postData(char * data, size_t length, char * topic);

int8_t string_switch(char * input_array[], uint8_t array_len, char * myTopic);

/**
 * @brief Takes a struct, it's name, and the field name and posts the data to the respective topic
 * @param struct_name char* The name of the struct
 * @param messgage_description pb_msgdesc_t* The proto buffer struct description
 * @param data void* The data to be posted
 */
void postDeviceDataPoint(char* struct_name, pb_msgdesc_t* descriptor, void * data, uint32_t length);

uint8_t checkDataEvent(void);

void getTopic( char * dest_topic_buffer, uint8_t * topiclen);

void getData( char * dest_data_buffer, uint8_t * datalen);

#endif