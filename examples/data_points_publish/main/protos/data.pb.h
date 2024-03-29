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

#ifndef PB_DATA_PB_H_INCLUDED
#define PB_DATA_PB_H_INCLUDED
#include <pb.h>

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct _MPU {
    int32_t ax;
    int32_t ay;
    int32_t az;
    int32_t gx;
    int32_t gy;
    int32_t gz;
} MPU;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define MPU_init_default                         {0, 0, 0, 0, 0, 0}
#define MPU_init_zero                            {0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define MPU_ax_tag                               1
#define MPU_ay_tag                               2
#define MPU_az_tag                               3
#define MPU_gx_tag                               4
#define MPU_gy_tag                               5
#define MPU_gz_tag                               6

/* Struct field encoding specification for nanopb */
#define MPU_FIELDLIST(X, a) \
X(a, STATIC,   SINGULAR, INT32,    ax,                1) \
X(a, STATIC,   SINGULAR, INT32,    ay,                2) \
X(a, STATIC,   SINGULAR, INT32,    az,                3) \
X(a, STATIC,   SINGULAR, INT32,    gx,                4) \
X(a, STATIC,   SINGULAR, INT32,    gy,                5) \
X(a, STATIC,   SINGULAR, INT32,    gz,                6)
#define MPU_CALLBACK NULL
#define MPU_DEFAULT NULL

extern const pb_msgdesc_t MPU_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define MPU_fields &MPU_msg

/* Maximum encoded size of messages (where known) */
#define MPU_size                                 66

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
