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

#include "golain_constants.h"
#include "golain_types.h"
#include "golain.h"
#include "golain_hal.h"
#include "golain_encode.h"

#include <sys/param.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "esp_tls.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"
#include "logs.pb.h"
#include "device_health.pb.h"



#ifdef CONFIG_GOLAIN_MQTT_OTA
#include <esp_ota_ops.h>
#include <string.h>
#include <esp_partition.h>
#endif

#ifdef CONFIG_GOLAIN_WIFI
#include "esp_wifi.h"
#include <esp_wifi_types.h>
#endif

#ifdef CONFIG_GOLAIN_BLE
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

#define TAG     "GOLAIN_HAL"

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

/*-----------------------------------Variables----------------------------------------------*/

nvs_handle_t _golain_device_health_nvs_handle;
esp_mqtt_client_handle_t _golain_mqtt_client; 


// wifi
#ifdef CONFIG_GOLAIN_WIFI
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;
#endif

golain_t* _golain;

golain_err_t golain_hal_init(golain_t * golain){
    _golain=golain;
    return GOLAIN_OK;
}

/* ---------------------------------- MQTT ---------------------------------------*/

static void golain_hal_mqtt_event_handler(void *golain_client, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%ld", base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    _golain_mqtt_client = event->client;
    golain_t *_golain_client = (golain_t*)golain_client;
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id) {

    case MQTT_EVENT_CONNECTED:

        _golain_client->mqtt_is_connected = 1;
        ESP_LOGI(TAG, "Connected to Golain");
        msg_id = _golain_hal_mqtt_subscribe(GOLAIN_SHADOW_READ_TOPIC, 1);
        ESP_LOGD(TAG, "Sent subscribe successful, msg_id=%d", msg_id);
        
        #ifdef CONFIG_GOLAIN_MQTT_OTA
        msg_id = _golain_hal_mqtt_subscribe(GOLAIN_OTA_TOPIC, 2);
        #endif

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGE(TAG, "MQTT_EVENT_DISCONNECTED");
        _golain_client->mqtt_is_connected = 0;
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGD(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGD(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGD(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        golain_mqtt_process_message(_golain_client, event->topic, event->topic_len, event->data, event->data_len, event->total_data_len);
        ESP_LOGD(TAG, " %.*s %.*s", event->data_len, event->data, event->topic_len, event->topic);
        break;

    case MQTT_EVENT_ERROR:
        _golain_client->mqtt_is_connected = 0;
        ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            ESP_LOGE(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGE(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGE(TAG, "Last captured errno : %d (%s)",  event->error_handle->esp_transport_sock_errno,
            strerror(event->error_handle->esp_transport_sock_errno));
        } else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) {
            ESP_LOGE(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        } else {
            ESP_LOGE(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;

    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}


golain_err_t _golain_hal_mqtt_init(golain_t * _golain_client){
    golain_err_t err = GOLAIN_OK;  

    #if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)

    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = GOLAIN_MQTT_BROKER_URL,
        .broker.verification.certificate = (const char*)_golain_client->config->root_ca_cert_start,
        .credentials = {
            .client_id = CONFIG_GOLAIN_DEVICE_NAME,
            .authentication = {
                .certificate = (const char*)_golain_client->config->device_cert,
                .key = (const char*)_golain_client->config->device_pvt_key,
            }
        }
    };
    
    #else 

    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = GOLAIN_MQTT_BROKER_URI,
        .port = GOLAIN_MQTT_BROKER_PORT,
        .client_cert_pem = (const char*)_golain_client->config->device_cert,
        .client_key_pem = (const char*)_golain_client->config->device_pvt_key,
        .client_id = CONFIG_GOLAIN_DEVICE_NAME,
        .cert_pem = (const char *)_golain_client->config->root_ca_cert_start,
        
    };

    #endif

    esp_err_t res = esp_tls_init_global_ca_store();
    res = esp_tls_set_global_ca_store((unsigned char *)_golain_client->config->root_ca_cert_start, _golain_client->config->root_ca_cert_len);

    if(res != 0){
        ESP_LOGE(TAG,"Error code: 0x%08x\n", res);
        return GOLAIN_MQTT_CONNECT_FAIL;
    }

    ESP_LOGI(TAG, "[APP] Free memory: %ld bytes", esp_get_free_heap_size());
    _golain_mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    /* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
    esp_mqtt_client_register_event(_golain_mqtt_client, ESP_EVENT_ANY_ID, golain_hal_mqtt_event_handler, _golain_client);
    
    return err;
}

golain_err_t _golain_hal_mqtt_connect(void){
    int err = esp_mqtt_client_start(_golain_mqtt_client);
    if(err){
        ESP_LOGE(TAG, "Error code: %d", err);
        return GOLAIN_MQTT_CONNECT_FAIL;
    }
    return GOLAIN_OK;
}

golain_err_t _golain_hal_mqtt_disconnect(void){
    int err = esp_mqtt_client_disconnect(_golain_mqtt_client);
    if(err){
        ESP_LOGD(TAG, "Error code: %d", err);
        return GOLAIN_MQTT_DISCONNECT_FAIL;
    }
    return GOLAIN_OK;
    
}

golain_err_t _golain_hal_mqtt_subscribe(const char *topic, uint8_t qos){
    int err = esp_mqtt_client_subscribe(_golain_mqtt_client, topic, qos);
    if(err){
        ESP_LOGD(TAG, "Error code: %d", err);
        return GOLAIN_MQTT_SUBSCRIBE_FAIL;
    }
    return GOLAIN_OK;
}


golain_err_t _golain_hal_mqtt_unsubscribe(const char *topic){
    int err = esp_mqtt_client_unsubscribe(_golain_mqtt_client, topic);
    if(err){
        ESP_LOGD(TAG, "Error code: %d", err);
        return GOLAIN_MQTT_UNSUBSCRIBE_FAIL;
    }
    return GOLAIN_OK;
}

golain_err_t _golain_hal_mqtt_publish(const char *topic, const char *data, uint16_t data_len, uint8_t qos, bool retain){
    golain_err_t err = esp_mqtt_client_publish(_golain_mqtt_client, topic, data, data_len, qos, retain);
    if(err == -1){
        ESP_LOGE(TAG, "Error publishing to %s  Returned: %d", topic, err);
        return GOLAIN_MQTT_PUBLISH_FAIL;
    }
    return GOLAIN_OK;
}

/*-----------------------------------------WIFI---------------------------------------------------------*/
#ifdef CONFIG_GOLAIN_WIFI

static void golain_wifi_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < 5) { // Tries to connect 5 times
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

golain_err_t _golain_hal_wifi_init(void){
    

    s_wifi_event_group = xEventGroupCreate();
    wifi_config_t wifi_config;
    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &golain_wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &golain_wifi_event_handler, NULL, &instance_got_ip));
    esp_err_t ret = esp_wifi_get_config(WIFI_IF_STA, &wifi_config);
    if (ret == ESP_OK && strlen((char*)wifi_config.sta.password) > 7)
    {
        ESP_LOGI(TAG, "NVS %s" ,wifi_config.sta.ssid);
        ESP_LOGI(TAG, "NVS %s" ,wifi_config.sta.password);
        // for(uint8_t ffs = 0; ffs < 33 ; ffs += 1){
        //     printf(" %02x", wifi_config.sta.ssid[ffs]);
        // }
        // printf("\n");
        // for(uint8_t ffs = 0 ; ffs < 64 ; ffs += 1){
        //     printf(" %02x", wifi_config.sta.password[ffs]);
        // }
        printf("\n");
    }
    else{
        memset(wifi_config.sta.ssid, 0x00, 32);
        memset(wifi_config.sta.password, 0x00, 64);
        memcpy(wifi_config.sta.ssid, CONFIG_WIFI_SSID, strlen(CONFIG_WIFI_SSID));
        memcpy(wifi_config.sta.password, CONFIG_WIFI_PASSWORD, strlen(CONFIG_WIFI_PASSWORD));
    }

    wifi_config.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH; 
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 wifi_config.sta.ssid, wifi_config.sta.password);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 wifi_config.sta.ssid, wifi_config.sta.password);
        return GOLAIN_FAIL;
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
        return GOLAIN_FAIL;
    }

    return GOLAIN_OK;
}

golain_err_t _golain_hal_wifi_disconnect(void){

    if(esp_wifi_disconnect() != 0){
        return GOLAIN_FAIL;
    }

    return GOLAIN_OK;
}
#endif

/*-----------------------------------------SHADOW FUNCTIONS---------------------------------------------------------*/

golain_err_t _golain_hal_shadow_persistent_write(uint8_t * buff, size_t size){
    
    golain_err_t shadow_err;
    esp_err_t err = nvs_open(GOLAIN_SHADOW_NVS_KEY, NVS_READWRITE, &_golain_device_health_nvs_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised.");
        return NVS_NOT_INIT;
    } 

    err = nvs_set_blob(_golain_device_health_nvs_handle, GOLAIN_SHADOW_NVS_KEY, buff, size);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be updated");
        
        shadow_err = NVS_UPDATE_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Updated");
        
        shadow_err = GOLAIN_OK;
    }

    nvs_commit(_golain_device_health_nvs_handle);
    nvs_close(_golain_device_health_nvs_handle);
    return shadow_err;
}

golain_err_t _golain_hal_shadow_persistent_read(uint8_t * buff, size_t size){
    golain_err_t shadow_err;
    esp_err_t err = nvs_open(GOLAIN_SHADOW_NVS_KEY, NVS_READWRITE, &_golain_device_health_nvs_handle);
    if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised.");
        return NVS_NOT_INIT;
    } 

    size_t len = size;
    err = nvs_get_blob(_golain_device_health_nvs_handle, GOLAIN_SHADOW_NVS_KEY, buff, &len);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be read %d", err);
        shadow_err = NVS_READ_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Read");
        shadow_err = GOLAIN_OK;
    }

    nvs_close(_golain_device_health_nvs_handle);
    return shadow_err;
}

/*-----------------------------------------OTA FUNCTIONS---------------------------------------------------------*/
#ifdef CONFIG_GOLAIN_MQTT_OTA

#define OTA_TAG "GOLAIN_MQTT_OTA"

long _golain_ota_total_len = 0;
long _golain_ota_current_len = 0;
uint8_t _golain_ota_status = 0;
esp_ota_handle_t _golain_ota_handle = 0;

golain_err_t _golain_hal_ota_update(int event_total_data_len, char* event_data, int event_data_len)
{
    _golain_ota_current_len = _golain_ota_current_len + event_data_len;
    ESP_LOGI(OTA_TAG, "current length %ld  total length %ld", _golain_ota_current_len, _golain_ota_total_len);

    // Check if the message is an OTA update payload

    // Get the OTA partition info
    const esp_partition_t *partition = esp_ota_get_next_update_partition(NULL);
    if (partition == NULL)
    {
        ESP_LOGI(OTA_TAG, "Error: could not find a valid OTA partition");
        // Error: could not find a valid OTA partition
        esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_FAILED, "1", 1, 0, 0);
        return GOLAIN_ERR_NOT_SUPPORTED;
    }
    if (!_golain_ota_status)
    {
        // Start the OTA update process
        if (esp_ota_begin(partition, OTA_SIZE_UNKNOWN, &_golain_ota_handle) != ESP_OK)
        {
            // Error: could not start OTA update
            ESP_LOGI(OTA_TAG, "Error: could not start OTA update");
            esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_FAILED, "1", 1, 0, 0);
            return GOLAIN_ERR_INVALID_STATE;
        }
        _golain_ota_status = 1;
    }

    // Write the OTA update payload to the OTA partition
    esp_err_t writeError = esp_ota_write(_golain_ota_handle, (void *)event_data, event_data_len);
    if (writeError != ESP_OK)
    {
        ESP_LOGI(OTA_TAG, "Error: could not write OTA update payload to partition");
        ESP_LOGI(OTA_TAG, "%s", esp_err_to_name(writeError));
        esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_FAILED, "1", 1, 0, 0);
        return GOLAIN_ERR_NOT_FINISHED;
    }
    if (_golain_ota_total_len == _golain_ota_current_len)
    {
        ESP_LOGI(OTA_TAG, "Time to finalize the OTA ");
        // Finalize the OTA update
        if (esp_ota_end(_golain_ota_handle) != ESP_OK)
        {
            // Error: could not finalize OTA update
            esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_FAILED, "1", 1, 0, 0);
            return GOLAIN_FAIL;
        }
        esp_err_t err = esp_ota_set_boot_partition(partition);
        // Reboot the device to activate the new firmware
        if (err != ESP_OK)
        {
            ESP_LOGI(OTA_TAG, "Error: could not set boot partition");
            esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_FAILED, "1", 1, 0, 0);
            return GOLAIN_FAIL;
        }
        esp_mqtt_client_publish(_golain_mqtt_client, GOLAIN_OTA_STATUS_SUCCESS, "1", 1, 0, 0);
        esp_restart();
        return GOLAIN_OK;
    }

    return GOLAIN_OK;
}

#endif

/*----------------------------------------------------BLE Fucntions ------------------------------------------------------------*/
#ifdef CONFIG_GOLAIN_BLE
// BLE event handling
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type)
    {
    // Advertise if connected
    case BLE_GAP_EVENT_CONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT CONNECT %s", event->connect.status == 0 ? "OK!" : "FAILED!");
        if (event->connect.status != 0)
        {
            ble_app_advertise();
        }
        break;
    // Advertise again after completion of the event
    case BLE_GAP_EVENT_ADV_COMPLETE:
        ESP_LOGI("GAP", "BLE GAP EVENT");
        ble_app_advertise();
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        ESP_LOGI("GAP", "BLE GAP EVENT DISCONNECT");
        ble_app_advertise();
        break;

    default:
        break;
    }

    return 0;
}

// Define the BLE connection
void ble_app_advertise(void)
{
    // GAP - device name definition
    struct ble_hs_adv_fields fields;
    const char *device_name;
    memset(&fields, 0, sizeof(fields));
    device_name = ble_svc_gap_device_name(); // Read the BLE device name
    fields.name = (uint8_t *)device_name;
    fields.name_len = strlen(device_name);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    // GAP - device connectivity definition
    struct ble_gap_adv_params adv_params;
    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND; // connectable or non-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN; // discoverable or non-discoverable
    ble_gap_adv_start(ble_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
}

// The application
void ble_app_on_sync(void)
{
    ble_hs_id_infer_auto(0, &ble_addr_type); // Determines the best address type automatically
    ble_app_advertise();                     // Define the BLE connection
}

// The infinite task
void host_task(void *param)
{
    nimble_port_run(); // This function will return only when nimble_port_stop() is executed
}

static int _golain_ble_shadow_read_cb(uint16_t con_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    size_t size_encoded;
    _golain_shadow_get_trimmed_shadow_buffer(_golain, &size_encoded);
    os_mbuf_append(ctxt->om, _golain->config->shadow_buffer, size_encoded);
    return 0;
}

static int _golain_ble_shadow_write_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    _golain_shadow_update_from_buffer(_golain, ctxt->om->om_data,  ctxt->om->om_len); // Updating the shadow from the rceived buffer
    golain_mqtt_post_shadow(_golain);
    
    ESP_LOGI(TAG, "Data from the SHADOW client: %s\n", _golain->config->shadow_buffer);
    return 0;
}

#ifdef CONFIG_GOLAIN_CONSUMER_ENABLED

static int _golain_ble_associate_user_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    golain_mqtt_post_user_assoc(_golain, ctxt->om->om_data,  ctxt->om->om_len); //Posting user assoc on receive
    printf("Data from the USER client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

#endif


#ifdef CONFIG_GOLAIN_WIFI

void split_wifi_creds_by_token(char* input_array, size_t input_len, char* wifi_ssid, char* wifi_pass, const char * separator) {
    memset(wifi_ssid, '\0', 33);
    memset(wifi_ssid, '\0', 64);
    for(int i = 0; i < input_len; i++){
        if(input_array[i] == *separator){
            strncpy(wifi_ssid, input_array, i);
            strncpy(wifi_pass, input_array+(i+1), input_len-(i+1));
            printf("%d; %d\n", strlen(wifi_ssid), strlen(wifi_pass));
            return;
        }
    }
}

char _golain_ble_wifi_rcvbuff[32+64+1]; // 32 SSID, 63 PASS, 2: null + 2x separator
char newssid[32];
char newpass[64];
#define GOLAIN_WIFI_CREDS_SEPARATION_CHAR ","

static int _golain_ble_configure_wifi_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    // printf("Data from the WIFI client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    
    wifi_config_t wifi_config;

    esp_wifi_get_config(WIFI_IF_STA, &wifi_config);

    memset(_golain_ble_wifi_rcvbuff, '\0', (32+64+1));
    strncpy(_golain_ble_wifi_rcvbuff, (char*)ctxt->om->om_data, ctxt->om->om_len);


    split_wifi_creds_by_token(_golain_ble_wifi_rcvbuff, ctxt->om->om_len, newssid, newpass, GOLAIN_WIFI_CREDS_SEPARATION_CHAR);
    memset(wifi_config.sta.ssid, '\0', 32);
    memset(wifi_config.sta.password, '\0', 64);
    strncpy((char *)wifi_config.sta.ssid, newssid, strlen((char*)newssid));
    strncpy((char *)wifi_config.sta.password, newpass, strlen((char*)newpass));

    wifi_config.sta.threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD;
    wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_BOTH; 
        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    printf("SSID: %s, Password: %s", wifi_config.sta.ssid, wifi_config.sta.password); 

    esp_wifi_disconnect();
    esp_wifi_connect();
    return 0;
}

#endif

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {.type = BLE_GATT_SVC_TYPE_PRIMARY,
     .uuid = BLE_UUID16_DECLARE(0x3F23),                 // Define UUID for golain device type
     .characteristics = (struct ble_gatt_chr_def[]){
        { //  read shadow
            .uuid = BLE_UUID16_DECLARE(0x5FDA),
            .flags = BLE_GATT_CHR_F_READ,
            .access_cb = _golain_ble_shadow_read_cb,
        },
        { // write shadow
            .uuid = BLE_UUID16_DECLARE(0x5FDB),
            .flags = BLE_GATT_CHR_F_WRITE,
            .access_cb = _golain_ble_shadow_write_cb,
        },
        #ifdef CONFIG_GOLAIN_CONSUMER_ENABLED
        { // associate user
            .uuid = BLE_UUID16_DECLARE(0x5FDC),
            .flags = BLE_GATT_CHR_F_WRITE,
            .access_cb = _golain_ble_associate_user_cb,
        },
        #endif
        #ifdef CONFIG_GOLAIN_WIFI
        { // configure wifi
            .uuid = BLE_UUID16_DECLARE(0x5FDD),
            .flags = BLE_GATT_CHR_F_WRITE,
            .access_cb = _golain_ble_configure_wifi_cb,
        },
        #endif
        {0}
        }
    },
    {0}
};

golain_err_t golain_hal_ble_init(golain_t* golain){
    #if ESP_IDF_VERSION <= ESP_IDF_VERSION_VAL(5, 0, 0)
    esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
    #endif
    nimble_port_init();                        // 3 - Initialize the host stack
    ble_svc_gap_device_name_set(CONFIG_GOLAIN_DEVICE_NAME); // 4 - Initialize NimBLE configuration - server name
    ble_svc_gap_init();                        // 4 - Initialize NimBLE configuration - gap service
    ble_svc_gatt_init();                       // 4 - Initialize NimBLE configuration - gatt service
    ble_gatts_count_cfg(gatt_svcs);            // 4 - Initialize NimBLE configuration - config gatt services
    ble_gatts_add_svcs(gatt_svcs);             // 4 - Initialize NimBLE configuration - queues gatt services.
    ble_hs_cfg.sync_cb = ble_app_on_sync;      // 5 - Initialize application
    nimble_port_freertos_init(host_task);      // 6 - Run the thread
    return GOLAIN_OK;
}

#endif // CONFIG_GOLAIN_BLE

/*------------------------------------------------------------------------Persistent Logs----------------------------------------------------*/
#ifdef CONFIG_GOLAIN_CLOUD_LOGGING
golain_err_t _golain_hal_p_log_check_nvs_errors(esp_err_t err)
{
    switch (err)
    {
    case ESP_OK:
        printf("Done\n");
        return GOLAIN_OK;
    case ESP_ERR_NVS_NOT_FOUND:
        printf("The value is not initialized yet!\n");
        return NVS_READ_FAIL;
    default:
        printf("Error (%s) reading!\n", esp_err_to_name(err));
        return GOLAIN_FAIL;
    }
}

golain_err_t _golain_hal_p_log_write_to_nvs(uint8_t *data, size_t len)
{
    // open nvs_flash in readwrite mode
    esp_err_t err;
    nvs_handle_t p_log_handle;
    err = nvs_open("p_logs", NVS_READWRITE, &p_log_handle);
    if (err != ESP_OK)
    {
        return GOLAIN_FAIL;
    }

    // get last log id
    int32_t last_log_id;
    if (nvs_get_i32(p_log_handle, "last_log_id", &last_log_id) ==
        ESP_ERR_NVS_NOT_FOUND)
    {
        last_log_id = 0;
    }

    if (last_log_id == CONFIG_GOLAIN_MAX_PERSISTENT_LOGS)
        last_log_id = 0;
    else
        last_log_id++;

    // write to nvs flash
    char key[2];
    sprintf(key, "%ld", last_log_id);
    err = nvs_set_blob(p_log_handle, key, data, len);
    if (err != ESP_OK)
    {
        return NVS_UPDATE_FAIL;
    }

    // store current log id
    nvs_set_i32(p_log_handle, "last_log_id", last_log_id);

    // commit changes
    err = nvs_commit(p_log_handle);
    if (err != ESP_OK)
    {
        return NVS_UPDATE_FAIL;
    }
    nvs_close(p_log_handle);
#if CONFIG_PERSISTENT_LOGS_INTERNAL_LOG_LEVEL > 2
    ESP_LOGI(TAG, "Wrote to NVS: PLogID:%d", last_log_id);
#endif
    return GOLAIN_OK;
}

golain_err_t _golain_hal_p_log_write(esp_log_level_t level, const char *func,
                      const char *tag, const char *format, ...)
{
    // create log entry
    PLog plog = PLog_init_default;
    plog.level = level;

    plog.tag.funcs.encode = &golain_pb_encode_string;
    plog.tag.arg = (void *)tag;

    plog.function.funcs.encode = &golain_pb_encode_string;
    plog.function.arg = (void *)func;

    // get variable arguments
    va_list list;
    va_start(list, format);

    /*
     * buffer here is used to create the string log message as well as
     * store the encoded protobuf message
     */
    uint8_t buffer[256];
    memset(buffer, 0, 256);
    vsprintf((char *)buffer, format, list);
    // vprintf(format, list);
    va_end(list);

    plog.message.funcs.encode = &golain_pb_encode_string;
    plog.message.arg = buffer;

    plog.time_ms = esp_log_timestamp();

    // create output stream
    uint8_t out_buffer[256];
    pb_ostream_t ostream = pb_ostream_from_buffer(out_buffer, sizeof(buffer));
    pb_encode(&ostream, PLog_fields, &plog);

    // write to nvs flash
    esp_err_t err = _golain_hal_p_log_write_to_nvs(out_buffer, ostream.bytes_written);
    // clear buffer
    memset(out_buffer, 0, 256);

    // return error code
    if (err != ESP_OK)
    {
        ESP_LOGE("PERSISTENT_LOGS", "Error writing to NVS (%s)",
                 esp_err_to_name(err));
        return GOLAIN_FAIL;
    }
    return GOLAIN_OK;
}

golain_err_t _golain_hal_p_log_read_old_logs(uint8_t *buffer)
{
    esp_err_t err;
    nvs_handle_t p_log_handle;
    err = nvs_open("p_logs", NVS_READWRITE, &p_log_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error opening NVS (%s)", esp_err_to_name(err));
        return NVS_READ_FAIL;
    }
    for (uint8_t i = 0; i <= CONFIG_GOLAIN_MAX_PERSISTENT_LOGS; i++)
    {
        char key[4];
        sprintf(key, "%d", i);

#if CONFIG_PERSISTENT_LOGS_INTERNAL_LOG_LEVEL > 2
        ESP_LOGI(TAG, "Reading NVS key: %s", key);
#endif
        // get size of stored BLOB
        size_t len;
        err = nvs_get_blob(p_log_handle, key, NULL, &len);

#if CONFIG_PERSISTENT_LOGS_INTERNAL_LOG_LEVEL > 2
        ESP_LOGI(TAG, "Size of stored data: %d", len);
#endif

        if (len == 0)
        {
#if CONFIG_PERSISTENT_LOGS_INTERNAL_LOG_LEVEL > 2
            ESP_LOGI(TAG, "Nothing stored here.");
#endif
            continue;
        }
        // get stored BLOB
        err = nvs_get_blob(p_log_handle, key, buffer, &len);
        if (err != ESP_OK)
        {
            ESP_LOGE(TAG, "Error reading NVS (%s)", esp_err_to_name(err));
            continue;
        }
#if CONFIG_PERSISTENT_LOGS_INTERNAL_LOG_LEVEL > 2
        ESP_LOGI(TAG, "Read from NVS: PLogID:%d : %s", i, (char *)buffer);
#endif
        // _send_mqtt_message(buffer, len);
    }
    //   memset(buffer, 0, 256);
    nvs_close(p_log_handle);
    return GOLAIN_OK;
}

golain_err_t _golain_hal_p_log_get_number_of_logs(int32_t *num)
{
    esp_err_t err;
    nvs_handle_t p_log_handle;
    err = nvs_open("p_logs", NVS_READWRITE, &p_log_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Error opening NVS (%s)", esp_err_to_name(err));
        return NVS_READ_FAIL;
    }

    return nvs_get_i32(p_log_handle, "last_log_id", num);
}
#endif // CONFIG_GOLAIN_PERSISTENT_LOGS
/*-----------------------------------------------------------------------Device Health------------------------------------------------------*/
#ifdef GOLAIN_REPORT_DEVICE_HEALTH

golain_err_t _golain_hal_device_health_store(uint8_t *deviceHealthproto){
    nvs_handle_t _golain_device_health_nvs_handle;

    // Open
    esp_err_t err = nvs_open(GOLAIN_DEVICE_HEALTH_NVS_KEY, NVS_READWRITE, &_golain_device_health_nvs_handle);
    if (err != ESP_OK)
        return NVS_READ_FAIL;

    err = nvs_set_blob(_golain_device_health_nvs_handle, GOLAIN_DEVICE_HEALTH_NVS_KEY, deviceHealthproto, CONFIG_GOLAIN_DEVICE_HEALTH_BUFFER_SIZE);
    if (err != ESP_OK)
        return NVS_UPDATE_FAIL;

    nvs_close(_golain_device_health_nvs_handle);
    return GOLAIN_OK;
} 

int8_t _golain_hal_reset_counter(void){
    int8_t num = 0;
    nvs_handle_t _golain_device_health_nvs_handle;
    esp_err_t err;

    // Open
    err = nvs_open(GOLAIN_DEVICE_HEALTH_NVS_KEY, NVS_READWRITE, &_golain_device_health_nvs_handle);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }

    err = nvs_get_i8(_golain_device_health_nvs_handle, GOLAIN_DEVICE_HEALTH_RESET_COUNTER_NVS_KEY, &num);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }

    num++;
    err = nvs_set_i8(_golain_device_health_nvs_handle, GOLAIN_DEVICE_HEALTH_RESET_COUNTER_NVS_KEY, num);
    if (err != ESP_OK) {
      ESP_LOGE(TAG, "Error reading NVS (%s)", esp_err_to_name(err));
      return num;
    }
   
    nvs_close(_golain_device_health_nvs_handle);
    
    return num;
    
}
#endif // GOLAIN_REPORT_DEVICE_HEALTH