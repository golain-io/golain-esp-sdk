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

/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.8-dev */

#ifndef PB_PROTOS_DEVICE_HEALTH_PB_H_INCLUDED
#define PB_PROTOS_DEVICE_HEALTH_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Enum definitions */
typedef enum _rebootReason {
    rebootReason_ESP_RST_UNKNOWN = 0,
    rebootReason_ESP_RST_POWERON = 1,
    rebootReason_ESP_RST_EXT = 2,
    rebootReason_ESP_RST_SW = 3,
    rebootReason_ESP_RST_PANIC = 4,
    rebootReason_ESP_RST_INT_WDT = 5,
    rebootReason_ESP_RST_TASK_WDT = 6,
    rebootReason_ESP_RST_WDT = 7,
    rebootReason_ESP_RST_DEEPSLEEP = 8,
    rebootReason_ESP_RST_BROWNOUT = 9,
    rebootReason_ESP_RST_SDIO = 10
} rebootReason;

/* Struct definitions */
typedef struct _deviceHealth {
    int32_t numberOferrorsSinceLastReboot;
    int32_t numberOfReboots;
    int32_t deviceRevision;
    rebootReason lastRebootReason;
    pb_callback_t buildNumber;
    pb_callback_t userStringData;
    float userNumericData;
} deviceHealth;


#ifdef __cplusplus
extern "C" {
#endif

/* Helper constants for enums */
#define _rebootReason_MIN rebootReason_ESP_RST_UNKNOWN
#define _rebootReason_MAX rebootReason_ESP_RST_SDIO
#define _rebootReason_ARRAYSIZE ((rebootReason)(rebootReason_ESP_RST_SDIO+1))

#define deviceHealth_lastRebootReason_ENUMTYPE rebootReason


/* Initializer values for message structs */
#define deviceHealth_init_default                {0, 0, 0, _rebootReason_MIN, {{NULL}, NULL}, {{NULL}, NULL}, 0}
#define deviceHealth_init_zero                   {0, 0, 0, _rebootReason_MIN, {{NULL}, NULL}, {{NULL}, NULL}, 0}

/* Field tags (for use in manual encoding/decoding) */
#define deviceHealth_numberOferrorsSinceLastReboot_tag 1
#define deviceHealth_numberOfReboots_tag         2
#define deviceHealth_deviceRevision_tag          3
#define deviceHealth_lastRebootReason_tag        4
#define deviceHealth_buildNumber_tag             5
#define deviceHealth_userStringData_tag          6
#define deviceHealth_userNumericData_tag         7

/* Struct field encoding specification for nanopb */
#define deviceHealth_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    numberOferrorsSinceLastReboot,   1) \
X(a, STATIC,   SINGULAR, INT32,    numberOfReboots,   2) \
X(a, STATIC,   SINGULAR, INT32,    deviceRevision,    3) \
X(a, STATIC,   SINGULAR, UENUM,    lastRebootReason,   4) \
X(a, CALLBACK, SINGULAR, STRING,   buildNumber,       5) \
X(a, CALLBACK, SINGULAR, STRING,   userStringData,    6) \
X(a, STATIC,   SINGULAR, FLOAT,    userNumericData,   7)
#define deviceHealth_CALLBACK pb_default_field_callback
#define deviceHealth_DEFAULT NULL

extern const pb_msgdesc_t deviceHealth_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define deviceHealth_fields &deviceHealth_msg

/* Maximum encoded size of messages (where known) */
/* deviceHealth_size depends on runtime parameters */

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
