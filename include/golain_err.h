#ifndef __GOLAIN_ERR_H__
#define __GOLAIN_ERR_H__

typedef enum _golain_err_t
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
    GOLAIN_ERR_NOT_FINISHED = 0x10C,     /*!< There are items remained to retrieve */
    
    GOLAIN_ERR_INVALID_VERSION = 0x10A,  /*!< Version was invalid */
    GOLAIN_ERR_INVALID_MAC = 0x10B,      /*!< MAC address was invalid */

    GOLAIN_ERR_WIFI_BASE = 0x3000,      /*!< Starting number of WiFi error codes */
    GOLAIN_ERR_MESH_BASE = 0x4000,      /*!< Starting number of BLE error codes */
    GOLAIN_ERR_FLASH_BASE = 0x6000,     /*!< Starting number of NVS error codes */

} golain_err_t;

#endif