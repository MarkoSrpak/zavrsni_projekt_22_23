#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "max30003_common.h"
#include "max30003_ECG.h"

#include "TCPsocket.h"


void max3_synchronize(){
    max3_write_reg(MAX3_REG_SYNCH, 0x00);
}

void max3_FIFO_reset(){
    max3_write_reg(MAX3_REG_FIFO_RST, 0x00);
}

void max3_ECG_enable(uint8_t enable){
    max3_write_reg_bits(MAX3_REG_CNFG_GEN, 19, 19, enable);
}

void max3_FMSTR_set(uint8_t mode){
    max3_write_reg_bits(MAX3_REG_CNFG_GEN, 20, 21, mode);
}

void max3_ECG_rate_set(uint8_t mode){
    uint32_t fmstr = max3_read_reg_bits(MAX3_REG_CNFG_GEN, 20, 21);
    esp_err_t ret = ESP_OK;
    if((fmstr == 0 || fmstr == 1) && mode == 3){
        ret = ESP_ERR_INVALID_ARG;
    }
    if((fmstr == 2 || fmstr == 3) && (mode == 0 || mode == 1 || mode == 3)){
        ret = ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(ret);
    max3_write_reg_bits(MAX3_REG_CNFG_ECG, 22, 23, mode);
}

void max3_ECG_gain_set(uint8_t mode){
    max3_write_reg_bits(MAX3_REG_CNFG_ECG, 16, 17, mode);
}

void max3_ECG_digital_high_pass_filter_set(uint8_t mode){
    max3_write_reg_bits(MAX3_REG_CNFG_ECG, 14, 14, mode);
}

void max3_ECG_digital_low_pass_filter_set(uint8_t mode){
    max3_write_reg_bits(MAX3_REG_CNFG_ECG, 12, 13, mode);
}

uint32_t max3_ECG_read(){
    uint32_t ECG_data = max3_read_reg(MAX3_REG_ECG_FIFO);
    uint32_t ECG_sample = (ECG_data & 0xFFFFC0) >> 5;
    if(ECG_sample & 0x20000) ECG_sample |= 0xFFFC0000; //ako je 18 bitni negativan, pretvori u 32 bitni negativan
    uint8_t ETAG = (ECG_data & 0x38) >> 3;
 //   printf("ETAG %d\n", ETAG);
    if(ETAG == 0 || ETAG == 2){ //valid sample
        printf("%ld\n", ECG_sample);
        tcp_send_data(ECG_sample);
        if(ETAG == 0)
            return 1;
        else
            return 0;
    }
    if(ETAG == 1){ //fast - not valid data, valid time
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
