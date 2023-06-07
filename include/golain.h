/*
 * Copyright (c) 2023 Quoppo LLC
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

#ifndef _GOLAIN_H
#define _GOLAIN_H

#include <stdio.h>
#include <stdbool.h>

#include "pb.h"
#include "pb_encode.h"
#include "pb_decode.h"

#include "golain_constants.h"
#include "golain_types.h"

#define DATA_TOPIC(name) GOLAIN_DATA_TOPIC name

golain_err_t golain_hal_init(golain_t * _golain);

golain_err_t golain_init(golain_t *golain, golain_config_t *config);


/*-------------------------------------------------------MQTT-------------------------------------*/
golain_err_t golain_mqtt_process_message(golain_t* _golain, char* topic, size_t topic_len, char * data, size_t data_len, size_t total_len);

golain_err_t golain_mqtt_post_data_point(char* topic, const void* descriptor, void * data, uint32_t length);
golain_err_t golain_mqtt_post_shadow(golain_t*);

#ifdef CONFIG_GOLAIN_CONSUMER_ENABLED
golain_err_t golain_mqtt_post_user_assoc(golain_t*, uint8_t * buff, uint8_t len);
#endif

/*-------------------------------------------------------WIFI-------------------------------------*/
#ifdef CONFIG_GOLAIN_WIFI
golain_err_t golain_hal_wifi_init();
golain_err_t golain_hal_wifi_disconnect(void);
#endif

/*-------------------------------------------------------Shadow-----------------------------------*/
/// @brief Initialises the global device shadow from NVS. If it does not find a shadow, default state is use.
/// @param  Pointer to the golain config struct
/// @return OK if successful, otherwise an error code
golain_err_t golain_shadow_init(golain_t*);

/// @brief Updates the GLOBAL SHADOW with a received buffer 
/// @param buff Received buffer conatining the encoded device shadow
/// @param len Length of `buff`
/// @return GOLAIN_OK on success, error code on failure.
golain_err_t _golain_shadow_update_from_buffer(golain_t* _golain, uint8_t * buff, size_t len);


/// @brief Updates shadow buffer and NVS with the GLOBAL SHADOW
/// @return GOLAIN_OK if successful, otherwise, error codes will be received
golain_err_t golain_shadow_update(golain_t*);


golain_err_t _golain_shadow_get_trimmed_shadow_buffer(golain_t *, size_t* );

/*------------------------------------------------------Persistent Logs-------------------------------------*/


/*------------------------------------------------------Device Health---------------------------------------*/
#ifdef CONFIG_GOLAIN_REPORT_DEVICE_HEALTH
golain_err_t golain_device_health_encode_message(uint8_t *buffer, size_t buffer_size, size_t *message_length);


golain_err_t golain_device_health_decode_message(uint8_t *buffer, size_t message_length);



#endif
/*------------------------------------------------------Nanopb callbacks-------------------------------------*/
bool golain_pb_decode_string(pb_istream_t *stream, const pb_field_t *field, void **arg);

bool golain_pb_encode_string(pb_ostream_t *stream, const pb_field_t *field, void * const *arg) ;


#endif