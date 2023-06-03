#ifndef _GOLAIN_PERSISTENT_LOGS_H_
#define _GOLAIN_PERSISTENT_LOGS_H_
#include <stdio.h>
#include <stdlib.h>
#include "persistent_logs.h"


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