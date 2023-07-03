#ifndef MDL_WIFI_H
#define MDL_WIFI_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


#define ESP_WIFI_SSID "A20e"
#define ESP_WIFI_PASS "burek123"
//#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#define EXAMPLE_ESP_MAXIMUM_RETRY 5


void wifi_init();


#endif /*MDL_WIFI_H*/