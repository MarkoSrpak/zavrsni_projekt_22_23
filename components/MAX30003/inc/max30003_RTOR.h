#ifndef MAX30003_RTOR_H
#define MAX30003_RTOR_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

//00 on STATUS readback, default
//01 on RTOR readback
//10 self clear afterecg cycle
//11 do not use
void max3_RTOR_interrupt_behaviour(uint8_t mode);

//enable RTOR detection
void max3_RTOR_enable(uint8_t enable);

//read RTOR register
uint32_t max3_RTOR_read();

#endif /*MAX30003_RTOR_H*/