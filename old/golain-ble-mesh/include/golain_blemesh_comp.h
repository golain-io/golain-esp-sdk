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