//prvi test - blinky sa MINIMALNO stvari dodanih

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "MDL_LED.h"

#include "print.h"
#include "maxChip.h"

static uint8_t s_led_state = 0;

void app_main(void){

    LED_init();
    max3_init();
    max3_info_verify();
    
    max3_software_reset();
    max3_ECG_enable(1);
    uint32_t ECG_status;
    while(1) {
        ECG_status = max3_read_reg(MAX3_REG_STATUS);
        if(ECG_status & 0x800000){
         //   printf("ECG_status %08lx\n", ECG_status);
            while(max3_ECG_read() != 0);
        }
      //  printf("%08lx\n", ECG_status);
      vTaskDelay(1);
    }
    while(1){
        if (s_led_state) {
            /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
            LED_on(16, 16, 0);
        } else {
            /* Set all LED off to clear all pixels */
            LED_off();
        }
        

    //    printf("svjetli: %d\n", s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(100);

    }
}