idf_component_register(SRCS "TCPsocket.c" "MDL_wifi.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_wifi nvs_flash)
