#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "led_strip.h"

#include "MDL_LED.h"

static led_strip_handle_t led_strip;

void LED_init(){

    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_GPIO,
        .max_leds = 1, // at least one LED on board
    };
    
    led_strip_rmt_config_t rmt_config = {
        .resolution_hz = 10 * 1000 * 1000, // 10MHz
    };
    
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    /* Set all LED off to clear all pixels */
    led_strip_clear(led_strip);
}

void LED_on(int r, int g, int b){
    led_strip_set_pixel(led_strip, 0, r, g, b);
    /* Refresh the strip to send data */
    led_strip_refresh(led_strip);
}

void LED_off(){
    led_strip_clear(led_strip);
}