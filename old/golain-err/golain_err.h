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

#pragma once
#include <stdio.h>
#include <stdlib.h>

typedef enum golain_err_t
{

    GOLAIN_OK = 0,    /*!< esp_err_t value indicating success (no error) */
    GOLAIN_FAIL = -1, /*!< Generic esp_err_t code indicating failure */

    GOLAIN_ERR_NO_MEM = 0x101,           /*!< Out of memory */
    GOLAIN_ERR_INVALID_ARG = 0x102,      /*!< Invalid argument */
    GOLAIN_ERR_INVALID_STATE = 0x103,    /*!< Invalid state */
    GOLAIN_ERR_INVALID_SIZE = 0x104,     /*!< Invalid size */
    GOLAIN_ERR_NOT_FOUND = 0x105,        /*!< Requested resource not found */
    GOLAIN_ERR_NOT_SUPPORTED = 0x106,    /*!< Operation or feature not supported */
    GOLAIN_ERR_TIMEOUT = 0x107,          /*!< Operation timed out */
    GOLAIN_ERR_INVALID_RESPONSE = 0x108, /*!< Received response was invalid */
    GOLAIN_ERR_INVALID_CRC = 0x109,      /*!< CRC or checksum was invalid */
    GOLAIN_ERR_INVALID_VERSION = 0x10A,  /*!< Version was invalid */
    GOLAIN_ERR_INVALID_MAC = 0x10B,      /*!< MAC address was invalid */
    GOLAIN_ERR_NOT_FINISHED = 0x10C,     /*!< There are items remained to retrieve */

    GOLAIN_ERR_WIFI_BASE = 0x3000,      /*!< Starting number of WiFi error codes */
    GOLAIN_ERR_MESH_BASE = 0x4000,      /*!< Starting number of MESH error codes */
    GOLAIN_ERR_FLASH_BASE = 0x6000,     /*!< Starting number of flash error codes */
    GOLAIN_ERR_HW_CRYPTO_BASE = 0xc000, /*!< Starting number of HW cryptography module error codes */
    GOLAIN_ERR_MEMPROT_BASE = 0xd000,   /*!< Starting number of Memory Protection API error codes */

} golain_err_t;