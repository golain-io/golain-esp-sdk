#include "golain_hal.h"
#include "golain_constants.h"
#include "golain.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_event.h"
#include "esp_netif.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include <sys/param.h>

//Wifi Stuff
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

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

nvs_handle_t shadow_nvs_handle;
esp_mqtt_client_handle_t _golain_mqtt_client; 


// wifi
static int s_retry_num = 0;
static EventGroupHandle_t s_wifi_event_group;

golain_t* _golain;

golain_err_t golain_hal_init(golain_t * golain){
    _golain=golain;
    return GOLAIN_OK;
}

/* ---------------------------------- MQTT ---------------------------------------*/

static void golain_hal_mqtt_event_handler(void *golain_client, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);
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
        golain_mqtt_process_message(_golain_client, event->topic, event->topic_len, event->data, event->data_len);
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
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = GOLAIN_MQTT_BROKER_URI,
        .port = GOLAIN_MQTT_BROKER_PORT,
        .client_cert_pem = (const char*)_golain_client->config->device_cert,
        .client_key_pem = (const char*)_golain_client->config->device_pvt_key,
        .client_id = CONFIG_GOLAIN_DEVICE_NAME,
        .cert_pem = (const char *)_golain_client->config->broker_cert,
        
    };

    esp_err_t res = esp_tls_init_global_ca_store();
    res = esp_tls_set_global_ca_store((unsigned char *)_golain_client->config->root_ca_cert_start, _golain_client->config->root_ca_cert_end - _golain_client->config->root_ca_cert_start);

    if(res != 0){
        ESP_LOGE(TAG,"Error code: 0x%08x\n", res);
        return GOLAIN_MQTT_CONNECT_FAIL;
    }

    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
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
    if(err != 0){
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

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &golain_wifi_event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &golain_wifi_event_handler, NULL, &instance_got_ip));
    wifi_config_t wifi_config = {
        .sta = {
                .ssid = CONFIG_WIFI_SSID,
                .password = CONFIG_WIFI_PASSWORD,
                .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
                .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            },
    };
    
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
                 CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
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
    esp_err_t err = nvs_open(NVS_SHADOW_KEY, NVS_READWRITE, &shadow_nvs_handle);
     if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised.");
        return NVS_NOT_INIT;
    } 

    err = nvs_set_blob(shadow_nvs_handle, NVS_SHADOW_KEY, buff, size);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be updated");
        
        shadow_err = NVS_UPDATE_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Updated");
        
        shadow_err = GOLAIN_OK;
    }

    nvs_commit(shadow_nvs_handle);
    nvs_close(shadow_nvs_handle);
    return shadow_err;
}

golain_err_t _golain_hal_shadow_persistent_read(uint8_t * buff, size_t size){
    golain_err_t shadow_err;
    esp_err_t err = nvs_open(NVS_SHADOW_KEY, NVS_READWRITE, &shadow_nvs_handle);
    if (err == ESP_ERR_NVS_NOT_INITIALIZED){
        ESP_LOGE(TAG, "NVS not initialised.");
        return NVS_NOT_INIT;
    } 

    size_t len = size;
    err = nvs_get_blob(shadow_nvs_handle, NVS_SHADOW_KEY, buff, &len);

    if(err != ESP_OK){
        ESP_LOGE(TAG, "NVS could not be read");
        shadow_err = NVS_READ_FAIL;
    }
    else{
        ESP_LOGI(TAG, "NVS Read");
        shadow_err = GOLAIN_OK;
    }

    nvs_close(shadow_nvs_handle);
    return shadow_err;
}


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
    os_mbuf_append(ctxt->om, shadow_buffer, _golain->config->shadow_size);
    return 0;
}

static int _golain_ble_shadow_write_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    printf("Data from the SHADOW client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

static int _golain_ble_associate_user_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    printf("Data from the USER client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}

static int _golain_ble_configure_wifi_cb(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt *ctxt, void *arg) {
    printf("Data from the WIFI client: %.*s\n", ctxt->om->om_len, ctxt->om->om_data);
    return 0;
}
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
        { // associate user
            .uuid = BLE_UUID16_DECLARE(0x5FDC),
            .flags = BLE_GATT_CHR_F_WRITE,
            .access_cb = _golain_ble_associate_user_cb,
        },
        { // configure wifi
            .uuid = BLE_UUID16_DECLARE(0x5FDD),
            .flags = BLE_GATT_CHR_F_WRITE,
            .access_cb = _golain_ble_configure_wifi_cb,
        },
        {0}
        }
    },
    {0}
};

golain_err_t golain_hal_ble_init(golain_t* golain){
    esp_nimble_hci_and_controller_init();      // 2 - Initialize ESP controller
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

#endif