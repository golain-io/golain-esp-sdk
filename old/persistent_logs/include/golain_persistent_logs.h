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