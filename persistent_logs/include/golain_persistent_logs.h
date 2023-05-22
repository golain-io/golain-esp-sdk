#ifndef _GOLAIN_PERSISTENT_LOGS_H_
#define _GOLAIN_PERSISTENT_LOGS_H_
#include <stdio.h>
#include "persistent_logs.h"

/**
 * @brief - Logs and stores the Info statements given as a param
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */
GOLAIN_LOG_I(tag, format, ...)
P_LOG_I(tag, format, ##__VA_ARGS__)

/**
 * @brief - Logs and stores the Error statements given as a param
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */
GOLAIN_LOG_E(tag, format, ...)
P_LOG_E(tag, format, ##__VA_ARGS__)

/**
 * @brief - Logs and stores the Warning statements given as a param
 *
 * @param tag A string which is used to trace the origin of log
 *
 * @param format Format in which data is to be logged and stored
 *
 * @param ... n Number of variable arguments that can be added as a part of log statement
 *
 */
GOLAIN_LOG_W(tag, format, ...)
P_LOG_W(tag, format, ##__VA_ARGS__)

/**
 * @brief This API reads the stored logs in protocol format and stores into the provided buffer.
 * 
 * @param buffer A uint8_t array used to store the individual log
 * 
*/
esp_err_t golain_read_old_logs(uint8_t *buffer);

/**
 * @brief This function checks the number of logs stored in NVS storage.
 * 
 * @param num a pointer to integer, which checks and gives out number of persisten logs in the NVS 
*/
esp_err_t golain_check_persistent_logs(int* num);

#endif