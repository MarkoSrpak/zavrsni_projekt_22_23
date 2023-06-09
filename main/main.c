#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "MDL_LED.h"

#include "max30003_common.h"
#include "max30003_ECG.h"
#include "max30003_RTOR.h"

#include "MDL_wifi.h"
#include "TCPsocket.h"

static uint8_t s_led_state = 0;

void app_main(void){

    wifi_init();
    tcp_socket_init();
    LED_init();
    max3_init();
    max3_info_verify();
    
    max3_software_reset();

    max3_FMSTR_set(0);
    max3_ECG_rate_set(0);
    max3_ECG_gain_set(0);
    max3_ECG_enable(1);

    max3_RTOR_enable(1);
    max3_RTOR_interrupt_behaviour(1);
    max3_synchronize();
    uint32_t ECG_status;
  //  float heart_rate = max3_RTOR_read();
    while(1) {
        ECG_status = max3_read_reg(MAX3_REG_STATUS);
        if(ECG_status & 0x800000){
         //   printf("ECG_status %08lx\n", ECG_status);
            while(1){
                uint32_t ECG_sample = 0;
                uint32_t result = max3_ECG_read(&ECG_sample);
                if(result == -1) break;
                printf("%ld\n", ECG_sample);
                tcp_send_data(ECG_sample);
                if(result == 0) break;
            }
        }
        if(ECG_status & 0x400){
         //   heart_rate = 1000.f * 60.f / (7.8125f * max3_RTOR_read());
          //  printf("%f\n", heart_rate);
        }
      //  printf("%08lx\n", ECG_status);
      vTaskDelay(1);
    }
    while(1){
        if (s_led_state) {
        //     Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color 
            LED_on(16, 16, 0);
        } else {
    //         Set all LED off to clear all pixels 
            LED_off();
        }

    //    printf("svjetli: %d\n", s_led_state);
        s_led_state = !s_led_state;
        vTaskDelay(100);
    }

    
}