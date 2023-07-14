#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

#define _OPEN_SYS_ITOA_EXT
#include <stdio.h>

#include "string.h"
#include "TCPsocket.h"


static const char *TAG = "TCP socket";

static int sock;

void tcp_socket_init(){
    char rx_buffer[128];
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    //---------- IPv4 config
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);
    addr_family = AF_INET;
    ip_protocol = IPPROTO_IP;
    //---------- Opening socket on port
    sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
    if (sock < 0) {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
      //  break;
    }
    ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
    if (err != 0) {
        ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
     //   break;
    }
    ESP_LOGI(TAG, "Successfully connected");
    //----------
}

void tcp_send_data(uint32_t data){
    char buffer[20];
    sprintf(buffer, "%ld\n", data);
    int err = send(sock, buffer, strlen(buffer), 0);
    if (err < 0) {
      //  ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
}

