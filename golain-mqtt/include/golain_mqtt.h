#ifndef _GOLAIN_MQTT_H_
#define _GOLAIN_MQTT_H_


#include "pb_encode.c"
#include "pb_decode.c"
#include "pb_common.c"
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
#include "esp_ota_ops.h"
#include <sys/param.h>





void mqtt_app_start();

void postData(char * data, int length, char * topic, esp_mqtt_client_handle_t client);

int8_t string_switch(char * input_array[], uint8_t array_len, char * myTopic);

void postDeviceDataPoint(char* struct_name, pb_msgdesc_t* descriptor, void * data, uint32_t length);

uint8_t checkDataEvent(void);

void getTopic( char * dest_topic_buffer, uint8_t * topiclen);

void getData( char * dest_data_buffer, uint8_t * datalen);

#endif