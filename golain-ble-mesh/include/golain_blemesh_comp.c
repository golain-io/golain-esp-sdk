#include "golain_blemesh_comp.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOSConfig.h"

//BLE Includes

#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "mesh/mesh.h"

#include "golain_blemesh_comp.h"

static const char *tag = "NimBLE_MESH";
void ble_store_config_init(void);

void (*bt_data_get_cb)(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf);

void (*bt_control_get_cb)(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf);

void (*bt_control_set_cb)(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf);
                           
#define BT_DBG_ENABLED (MYNEWT_VAL(BLE_MESH_DEBUG))

/* Company ID */
#define CID_VENDOR 0x05C3
#define STANDARD_TEST_ID 0x00
#define TEST_ID 0x01
static int recent_test_id = STANDARD_TEST_ID;

#define FAULT_ARR_SIZE 2

static bool has_reg_fault = true;


static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BT_MESH_RELAY_DISABLED,
    .beacon = BT_MESH_BEACON_ENABLED,
#if MYNEWT_VAL(BLE_MESH_FRIEND)
    .frnd = BT_MESH_FRIEND_ENABLED,
#endif
#if MYNEWT_VAL(BLE_MESH_GATT_PROXY)
    .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
#else
    .gatt_proxy = BT_MESH_GATT_PROXY_NOT_SUPPORTED,
#endif
    .default_ttl = 7,

    /* 3 transmissions with 20ms interval */
    .net_transmit = BT_MESH_TRANSMIT(2, 20),
    .relay_retransmit = BT_MESH_TRANSMIT(2, 20),
};

static int
fault_get_cur(struct bt_mesh_model *model,
              uint8_t *test_id,
              uint16_t *company_id,
              uint8_t *faults,
              uint8_t *fault_count)
{
    uint8_t reg_faults[FAULT_ARR_SIZE] = { [0 ... FAULT_ARR_SIZE - 1] = 0xff };

    ESP_LOGI(tag, "fault_get_cur() has_reg_fault %u\n", has_reg_fault);

    *test_id = recent_test_id;
    *company_id = CID_VENDOR;

    *fault_count = min(*fault_count, sizeof(reg_faults));
    memcpy(faults, reg_faults, *fault_count);

    return 0;
}

static int
fault_get_reg(struct bt_mesh_model *model,
              uint16_t company_id,
              uint8_t *test_id,
              uint8_t *faults,
              uint8_t *fault_count)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    ESP_LOGI(tag, "fault_get_reg() has_reg_fault %u\n", has_reg_fault);

    *test_id = recent_test_id;

    if (has_reg_fault) {
        uint8_t reg_faults[FAULT_ARR_SIZE] = { [0 ... FAULT_ARR_SIZE - 1] = 0xff };

        *fault_count = min(*fault_count, sizeof(reg_faults));
        memcpy(faults, reg_faults, *fault_count);
    } else {
        *fault_count = 0;
    }

    return 0;
}

static int
fault_clear(struct bt_mesh_model *model, uint16_t company_id)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    has_reg_fault = false;

    return 0;
}

static int fault_test(struct bt_mesh_model *model, uint8_t test_id, uint16_t company_id)
{
    if (company_id != CID_VENDOR) {
        return -BLE_HS_EINVAL;
    }

    if (test_id != STANDARD_TEST_ID && test_id != TEST_ID) {
        return -BLE_HS_EINVAL;
    }

    recent_test_id = test_id;
    has_reg_fault = true;
    bt_mesh_fault_update(bt_mesh_model_elem(model));

    return 0;
}

static const struct bt_mesh_health_srv_cb health_srv_cb = {
    .fault_get_cur = &fault_get_cur,
    .fault_get_reg = &fault_get_reg,
    .fault_clear = &fault_clear,
    .fault_test = &fault_test,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

static struct bt_mesh_model_pub health_pub;

static void health_pub_init(void)
{
    health_pub.msg  = BT_MESH_HEALTH_FAULT_MSG(0);
}

static struct bt_mesh_model root_models[] = {
    BT_MESH_MODEL_CFG_SRV(&cfg_srv),
    BT_MESH_MODEL_HEALTH_SRV(&health_srv, &health_pub),
};

static struct bt_mesh_model_pub vnd_model_pub;

/*
static void vnd_model_recv(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(3);

    console_printf("#vendor-model-recv - 1\n");

    console_printf("data:%s len:%d\n", bt_hex(buf->om_data, buf->om_len),
                   buf->om_len);

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(0x01, CID_VENDOR));
    os_mbuf_append(msg, buf->om_data, buf->om_len);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        console_printf("#vendor-model-recv: send rsp failed\n");
    }

    os_mbuf_free_chain(msg);
}

static void vnd_model_recv2(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf)
{
    struct os_mbuf *msg = NET_BUF_SIMPLE(3);

    console_printf("#vendor-model-recv- 2\n");

    console_printf("DATA:%s LENGTH:%d\n", bt_hex(buf->om_data, buf->om_len),
                   buf->om_len);

    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(0x01, CID_VENDOR));
    os_mbuf_append(msg, buf->om_data, buf->om_len);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        console_printf("#vendor-model-recv: send rsp failed\n");
    }

    os_mbuf_free_chain(msg);
}*/

//I really hate doping it this way but there doesn't seem to be any other choice
static void callback_1(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf){
    bt_data_get_cb(model,ctx, buf);
}
static void callback_2(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf){
    bt_control_get_cb(model,ctx, buf);
}

static void callback_3(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf){
    bt_control_set_cb(model,ctx, buf); 
}

static const struct bt_mesh_model_op vnd_model_op[] = {
        { BT_MESH_MODEL_OP_3(0x00, CID_VENDOR), 0, callback_1},
        BT_MESH_MODEL_OP_END,
};

static const struct bt_mesh_model_op vnd_model_op2[] = {
        { BT_MESH_MODEL_OP_3(0x01, CID_VENDOR), 0, callback_2 },       
        { BT_MESH_MODEL_OP_3(0x02, CID_VENDOR), 0, callback_3 },
        BT_MESH_MODEL_OP_END,
};

static struct bt_mesh_model vnd_models[] = {
    BT_MESH_MODEL_VND(CID_VENDOR, 0x1111, vnd_model_op,
              &vnd_model_pub, NULL),
    BT_MESH_MODEL_VND(CID_VENDOR, 0x2222, vnd_model_op2,
              &vnd_model_pub, NULL),
};

static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0, root_models, vnd_models),
};

static const struct bt_mesh_comp comp = {
    .cid = CID_VENDOR,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

static int output_number(bt_mesh_output_action_t action, uint32_t number)
{
    ESP_LOGI(tag, "OOB Number: %u\n", number);

    return 0;
}

static void prov_complete(u16_t net_idx, u16_t addr)
{
    ESP_LOGI(tag, "Local node provisioned, primary address 0x%04x\n", addr);
}

static const uint8_t dev_uuid[16] = MYNEWT_VAL(BLE_MESH_DEV_UUID);

static const struct bt_mesh_prov prov = {
    .uuid = dev_uuid,
    .output_size = 4,
    .output_actions = BT_MESH_DISPLAY_NUMBER | BT_MESH_BEEP | BT_MESH_VIBRATE | BT_MESH_BLINK,
    .output_number = output_number,
    .complete = prov_complete,
};

static void
blemesh_on_reset(int reason)
{
    BLE_HS_LOG(ERROR, "Resetting state; reason=%d\n", reason);
}

static void
blemesh_on_sync(void)
{
    int err;
    ble_addr_t addr;

    ESP_LOGI(tag, "Bluetooth initialized\n");

    /* Use NRPA */
    err = ble_hs_id_gen_rnd(1, &addr);
    assert(err == 0);
    err = ble_hs_id_set_rnd(addr.val);
    assert(err == 0);

    err = bt_mesh_init(addr.type, &prov, &comp);
    if (err) {
        ESP_LOGI(tag, "Initializing mesh failed (err %d)\n", err);
        return;
    }

    ESP_LOGI(tag, "Mesh initialized\n");

    if (IS_ENABLED(CONFIG_SETTINGS)) {
        settings_load();
    }

    if (bt_mesh_is_provisioned()) {
        ESP_LOGI(tag, "Mesh network restored from flash\n");
    }
}

void blemesh_host_task(void *param)
{
    ble_hs_cfg.reset_cb = blemesh_on_reset;
    ble_hs_cfg.sync_cb = blemesh_on_sync;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    health_pub_init();
    nimble_port_run();
    nimble_port_freertos_deinit();
}



esp_err_t send_message(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf, uint8_t * buffer, size_t len){
    struct os_mbuf *msg = NET_BUF_SIMPLE(3);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_3(0x01, CID_VENDOR));
    os_mbuf_append(msg, buffer, len);

    if (bt_mesh_model_send(model, ctx, msg, NULL, NULL)) {
        console_printf("#vendor-model-recv: send rsp failed\n");
    }

    os_mbuf_free_chain(msg);
    
    return ESP_OK;
}



esp_err_t bt_mesh_custom_start(MeshConfig ip_conf){
    bt_control_get_cb = ip_conf.user_control_get_cb;
    bt_control_set_cb = ip_conf.user_control_set_cb;
    bt_data_get_cb = ip_conf.user_data_get_cb;


        /* Initialize NVS — it is used to store PHY calibration data */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_nimble_hci_and_controller_init());
    nimble_port_init();

    ble_svc_gap_init();
    ble_svc_gatt_init();

    bt_mesh_register_gatt();
    /* XXX Need to have template for store */
    ble_store_config_init();

    nimble_port_freertos_init(blemesh_host_task);

    return 0;
}
