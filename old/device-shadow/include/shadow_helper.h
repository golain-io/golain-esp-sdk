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

#ifndef _SHADOW_HELPER_H_
#define _SHADOW_HELPER_H_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

#include "shadow.pb.h"

extern shadow Shadow;

typedef enum golain_err_t{
    GOLAIN_OK = 0, 
    NVS_NOT_INIT = 1, 
    NVS_UPDATE_FAIL = 2,
    NVS_EMPTY = 3,
    PB_UPDATE_FAIL = 4,
    PB_ENCODE_FAIL = 5,
    GENERIC_ERR = 10,
    
}golain_err_t;

/** @brief Config structure for shadow functionality
 * 
 * @param shadow_update_cb Callback function for when the shadow is updated
 * 
 * @param clean_on_error When set to HIGH or 1, clear NVS when invalid buffer is present. The buffer will 
 *                       be considered invalid when it cannot be decoded into the shadow protobuffer
 * 
*/
typedef struct _shadowcfg{
    
    
    void (*shadow_update_cb)(void);
    
    /** @brief When HIGH or 1, NVS will be cleared if invalid buffer is present
     * 
    */
    bool clean_on_error;
    

} ShadowCfg;

/** @brief A function to commit war crimes against the nvs by writing the global shadow onto it 
 * 
*/
golain_err_t UpdatewithStruct(void); //Done

/** 
 * @brief Update the global shadow and the NVS simultaneously with a buffer
* This is particularly useful when receiving an encoded shadow protobuff via 
* protocols like mqtt or ble
* 
* @param buff Buffer that contains encoded pb message
* 
* @param len Length of the pb message
    * 
*/
golain_err_t UpdatewithBuff(uint8_t * buff, size_t len); //Done

/** @brief Initialise the shadow service
 * 
 * @param temp_cfg Config structure required for initialising the shadow service
 * 
*/
golain_err_t InitDeviceShadow(ShadowCfg temp_cfg); //Done


golain_err_t updateNVS(uint8_t * buff, size_t len); //Done

/**
 * @brief Get the encoded version of the shadow protobuffer
 * 
 * @param buff The buffer you want to store the encoded protobuffer in 
 * 
 * @param buff_len Length of the above buffer
 * 
 * @param message_len Length of the shadow you are using 
*/
golain_err_t GetShadow(uint8_t * buff, size_t buff_len, size_t* encoded_size);

#endif