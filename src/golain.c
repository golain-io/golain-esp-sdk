#include "golain_err.h"
#include "golain.h"

#include <stdio.h>

golain_err_t golain_init(golain_t *golain, golain_config_t *config) {
    golain_err_t err = GOLAIN_OK;

    if (config == NULL || golain == NULL) {
       return GOLAIN_ERR_INVALID_ARG;
        
    }
    
    // set config to golain struct
    golain->config = config;
    golain->mqtt = NULL;

    #ifdef CONFIG_GOLAIN_BLE
    golain->ble = NULL;
    #endif


    // initialise mqtt client
    err = golain_mqtt_init(golain);
    if (err != GOLAIN_OK) {
        return err;
    }

    #ifdef CONFIG_GOLAIN_BLE
    // initialise ble client
    err = golain_ble_init(golain);
    if (err != GOLAIN_OK) {
        return err;
    }
    #endif

    return err;
}