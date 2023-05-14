#include "golain_mqtt.h"

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
//#include "protocol_examples_common.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>
#include "shadow_helper.h"
#include "shadow.pb.h"

#define DEVICE_SHADOW_TOPIC  CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/device-shadow" 
#define DEVICE_SHADOW_TOPIC_R  CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/device-shadow/r"
#define DEVICE_SHADOW_TOPIC_W  CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/device-shadow/w"
#define DEVICE_OTA_TOPIC     CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/ota"
#define DEVICE_DATA_TOPIC    CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/device-data"
#define USER_ASSOC_TOPIC    CONFIG_TOPIC_ROOT CONFIG_DEVICE_NAME "/user"


#define TAG "GOLAIN MQTT"


static uint8_t dataRcvFlag = 0;

char * split_topic[3]; 

extern Shadow shadow;

esp_mqtt_client_handle_t client; 
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


//-----------------------------------------------------------------------------------------------------------Only  used in this file


void splitintoarray(char * from_string, char ** str_array, char * split_char){
    char * temp_array = from_string;
    //memset(str_array[0], 0x00, 10);
    //memset(str_array[1], 0x00, 10);

    str_array[0] = strtok(temp_array, split_char);
    for(int i = 0; i< 3-1; i++){
        str_array[i+1] = strtok(NULL, split_char);
    }
}

//-----------------------------------------------------------------------------------------------------------------------------------

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    //char * rcv_topic = (char *)malloc(10);   
    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        

        
        msg_id = esp_mqtt_client_subscribe(client, DEVICE_SHADOW_TOPIC, 0);
        ESP_LOGI(TAG, "Sent subscribe successful, msg_id=%d", msg_id);
        
        // msg_id = esp_mqtt_client_subscribe(client, DEVICE_DATA_TOPIC, 0);
        // ESP_LOGI(TAG, "Sent subscribe successful, msg_id=%d", msg_id);
        
        
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        //postData("tay", 3 ,DEVICE_SHADOW_TOPIC);
        // msg_id = esp_mqtt_client_publish(client, "/Shadow", "data", 0, 0, 0);
        // ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        
        
        if(strncmp(event->topic, DEVICE_SHADOW_TOPIC_R, event->topic_len)==0){
            
            uint8_t shadowreadbuff[Shadow_size];
            pb_ostream_t ostream = pb_ostream_from_buffer(shadowreadbuff, Shadow_size);
            pb_encode(&ostream, Shadow_fields, &shadow);
            postShadow(shadowreadbuff, ostream.bytes_written);

        }
        else if(strncmp(event->topic, DEVICE_SHADOW_TOPIC_W, event->topic_len)==0){
            uint8_t shadowwritebuff[event->data_len];
            memcpy(shadowwritebuff, event->data, event->data_len);
            pb_istream_t istream = pb_istream_from_buffer(shadowwritebuff,event->data_len);
            if(!pb_decode(&istream, Shadow_fields, &shadow)){
                ESP_LOGE(TAG, "Decoding failed. E: %s", istream.errmsg);
            }
        }
        else{
        dataRcvFlag = 1;
        memcpy(dirtyTopicArray, event->topic, event->topic_len);
        rcvTopicLength = event->topic_len;

        memcpy(dirtyDataArray, event->data, event->data_len);
        rcvDataLength = event->data_len;
        }

        // //ESP_LOGI(TAG, "MQTT_EVENT_DATA Topic size: %d", event->topic_len);
        // rcv_topic = (char*)calloc((event->topic_len), sizeof(char));
        // memcpy(rcv_topic, event->topic, event->topic_len);

        // printf("TOPIC=%s\r\n", rcv_topic);
        // printf("DATA=%.*s\r\n", event->data_len, event->data);
        
        // ESP_LOGW(TAG,"%s %s", topics[0], topics[1]);
        
     
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

void mqtt_app_start(void){
    
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URI,
        .port = CONFIG_MQTT_PORT,
        .client_cert_pem = (const char*)mqtt_device_cert_pem_start,
        .client_key_pem = (const char*)mqtt_device_pvt_key_pem_start,
        .client_id = CONFIG_DEVICE_NAME,


    };

     esp_err_t res=esp_tls_init_global_ca_store();
    res = esp_tls_set_global_ca_store((unsigned char *)mqtt_root_ca_cert_pem_start, mqtt_root_ca_cert_pem_end-mqtt_root_ca_cert_pem_start); 
    if(res != 0){
        ESP_LOGI(TAG,"Error code: 0x%08x\n", res);
    }

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

int8_t string_switch(char * input_array[], uint8_t array_len, char * myTopic){
    printf("Comparing with %s \n", myTopic);
    
    uint8_t i =0;
    for(i = 0; i<array_len; i++){
        size_t temp_size = sizeof(input_array[i]);
        
        int temp = strncmp(myTopic, input_array[i], temp_size);
        printf("%s \n", input_array[i]);
        if(temp ==  0){
            printf("Matches with this %s", input_array[i]);
            return i;
        }
    }
    return -1;
}


esp_err_t postShadow(uint8_t * data, int length){
    
    int post_err = esp_mqtt_client_publish(client, DEVICE_SHADOW_TOPIC, (char*)data, length, 0, 0);

    ESP_LOGE(TAG, "Error publishing to %s  Returned: %d", DEVICE_SHADOW_TOPIC, post_err);

    return (esp_err_t) post_err;

}




void postData(char * data, size_t length, char * topic){ //Post already encoded datat point
    char topic_to_publish[sizeof(DEVICE_DATA_TOPIC)+sizeof(topic)];
    sprintf(topic_to_publish, "%s/%s", DEVICE_DATA_TOPIC, topic);
    esp_mqtt_client_publish(client, topic_to_publish, data, length, 0, 0);
    ESP_LOGI(TAG, "Published to topic: %s", topic);
}




esp_err_t postDeviceDataPoint(char* struct_name, const pb_msgdesc_t* descriptor, void * data, uint32_t length){
    esp_err_t err;    
    char * topic = (char*)calloc((sizeof(DEVICE_DATA_TOPIC)+sizeof(struct_name)), sizeof(char));
    sprintf(topic, "%s/%s", DEVICE_DATA_TOPIC, struct_name);
    
    ESP_LOGI(TAG, "Topic: %s", topic);
    uint8_t buffer[length];
    pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
    bool status = pb_encode(&stream, descriptor, data);
    if(!status){
        ESP_LOGE(TAG, "Encoding failed: %s", PB_GET_ERROR(&stream));
        err = ESP_FAIL;
        return err;
    }
    ESP_LOGI(TAG, "Encoded %d bytes", stream.bytes_written);
    err = esp_mqtt_client_publish(client, topic, (char*)buffer, stream.bytes_written, 0, 0);
    return err;
}

esp_err_t postUserAssoc(void * UAData, size_t len){
    esp_err_t err;
    ESP_LOGD(TAG, "Publishing : %s", (char*)UAData);
    err = esp_mqtt_client_publish(client, USER_ASSOC_TOPIC, (char*)UAData, len, 0, 0);
    return err;
}



uint8_t checkDataEvent(void){
    if(dataRcvFlag == 1){
        dataRcvFlag = 0;
        return 1;
    }
    return 0;
}


void getTopic( char * dest_topic_buffer, uint8_t * topiclen){
    memcpy(dest_topic_buffer, dirtyTopicArray, rcvTopicLength);
    memcpy(topiclen, &rcvTopicLength, sizeof(uint8_t));
}


void getData( char * dest_data_buffer, uint8_t * datalen){
    memcpy(dest_data_buffer, dirtyDataArray, rcvDataLength);
    memcpy(datalen, &rcvDataLength, sizeof(uint8_t));
}