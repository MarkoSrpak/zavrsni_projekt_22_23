#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "max30003_common.h"
#include "max30003_RTOR.h"

void max3_RTOR_enable(uint8_t enable){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_RTOR1);
    if(enable){
        max3_write_reg(MAX3_REG_CNFG_RTOR1, cnfg_reg | (1 << 15));
    }
    else{
        max3_write_reg(MAX3_REG_CNFG_RTOR1, cnfg_reg & ~(1 << 15));
    }
}

uint32_t max3_RTOR_read(){
    return max3_read_reg(MAX3_REG_RTOR) >> 10;
}

void max3_RTOR_interrupt_behaviour(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_MNGR_INT);
    cnfg_reg &= ~(0x30);
    cnfg_reg |= (mode & 0x3) << 4;
    max3_write_reg(MAX3_REG_MNGR_INT, cnfg_reg);
}