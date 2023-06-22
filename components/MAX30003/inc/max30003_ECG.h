#ifndef MAX30003_ECG_H
#define MAX30003_ECG_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


//write 0 to SYNCH register to initiate sync of max30003
void max3_synchronize();

//write 0 to FIFO_RST register to reset FIFO memories
void max3_FIFO_reset();

//EN_ECG bit in CNFG_GEN, true or false
void max3_ECG_enable(uint8_t enable);

//set master freq, default 0b00
void max3_FMSTR_set(uint8_t mode);

//set ECG rate, default 0b10
void max3_ECG_rate_set(uint8_t mode);

//set ECG gain, default 0b00
void max3_ECG_gain_set(uint8_t mode);

//0 = bypass (DC), 1 = 0.50Hz (default)
void max3_ECG_digital_high_pass_filter_set(uint8_t mode);

//00 = bypass
//01 = 40Hz (default)
//10 = 100Hz
//11 = 150Hz
void max3_ECG_digital_low_pass_filter_set(uint8_t mode);

//read ECG FIFO register
uint32_t max3_ECG_read();


#endif /*MAX30003_ECG_H*/