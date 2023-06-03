#ifndef _GOLAIN_BLEMESH_COMP_H_
#define _GOLAIN_BLEMESH_COMP_H_

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



typedef struct MeshConfig{
    /// @brief Callback for data get
    void (*user_data_get_cb)(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
    
    /// @brief Callback for control get
    void (*user_control_get_cb)(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
    
    /// @brief Callback for control set
    void (*user_control_set_cb)(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
   
    /// @brief Callback for user association 
    void (*user_user_assoc_cb)(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx, struct os_mbuf *buf);
   
} MeshConfig;

/// @brief Function to send a uint8_t buffer via the BLE mesh. Should only be used inside BLE Mesh callbacks that you set inside MeshConfig. model, ctx and buf are parameters passed by the Mesh.
/// @param model Parameter from callback
/// @param ctx Parameter from callback
/// @param buf Parameter from callback
/// @param buffer This is your actual that you want to send buf won't be sent but some of it's parameters are required
/// @param len Length of the buffer
/// @return 
esp_err_t send_message(struct bt_mesh_model *model,
                           struct bt_mesh_msg_ctx *ctx,
                           struct os_mbuf *buf,
                           uint8_t * buffer,
                           size_t len);


/// @brief Initalize the BLE mesh
/// @param ip_conf 
/// @return 
esp_err_t bt_mesh_custom_start(MeshConfig ip_conf);


#endif