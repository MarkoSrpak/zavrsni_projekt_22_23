#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "max30003_common.h"
#include "max30003_RTOR.h"

void max3_RTOR_enable(uint8_t enable){
    max3_write_reg_bits(MAX3_REG_CNFG_RTOR1, 15, 15, enable);
}

uint32_t max3_RTOR_read(){
    return max3_read_reg_bits(MAX3_REG_RTOR, 10, 23);
}

void max3_RTOR_interrupt_behaviour(uint8_t mode){
    esp_err_t ret = ESP_OK;
    if(mode == 3){
        ret = ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(ret);
    max3_write_reg_bits(MAX3_REG_MNGR_INT, 4, 5, mode);
}