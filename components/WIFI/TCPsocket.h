#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#define HOST_IP_ADDR "192.168.175.121"
#define PORT 3333

void tcp_socket_init();
void tcp_send_data();


#endif /*TCPSOCKET_H*/