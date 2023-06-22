#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "max30003_common.h"
#include "max30003_ECG.h"


void max3_synchronize(){
    max3_write_reg(MAX3_REG_SYNCH, 0x00);
}

void max3_FIFO_reset(){
    max3_write_reg(MAX3_REG_FIFO_RST, 0x00);
}

void max3_ECG_enable(uint8_t enable){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_GEN);
    if(enable){
        max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg | (1 << 19));
    }
    else{
        max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg & ~(1 << 19));
    }
}

void max3_FMSTR_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_GEN);
    cnfg_reg &= ~(0x300000);
    cnfg_reg |= (mode & 0x3) << 20;
    max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg);
}

void max3_ECG_rate_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0xC00000);
    cnfg_reg |= (mode & 0x3) << 22;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_gain_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x30000);
    cnfg_reg |= (mode & 0x3) << 16;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_digital_high_pass_filter_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x4000);
    cnfg_reg |= (mode & 0x1) << 14;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_digital_low_pass_filter_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x3000);
    cnfg_reg |= (mode & 0x3) << 12;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

uint32_t max3_ECG_read(){
    uint32_t ECG_data = max3_read_reg(MAX3_REG_ECG_FIFO);
    uint32_t ECG_sample = (ECG_data & 0xFFFFC0) >> 5;
    if(ECG_sample & 0x20000) ECG_sample |= 0xFFFC0000; //ako je 18 bitni negativan, pretvori u 32 bitni negativan
    uint8_t ETAG = (ECG_data & 0x38) >> 3;
 //   printf("ETAG %d\n", ETAG);
    if(ETAG == 0){ //valid sample
        printf("%ld\n", ECG_sample);
        return 1;
    }
    if(ETAG == 1){ //fast - not valid data, valid time
    }
    if(ETAG == 2){ //valid, EOF
        printf("%ld\n", ECG_sample);
        return 0;
    }
    if(ETAG == 3){ //fast - not valid data, valid time, EOF

    }
    //ETAG 4 and 5 are not used
    if(ETAG == 6){ //FIFO is empty
        return 0;
    }
    if(ETAG == 7){ //FIFO is in overflow
        max3_FIFO_reset();
        return 0;
    }
    return 0;
}
