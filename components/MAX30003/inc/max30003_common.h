#ifndef MAX30003_COMMON_H
#define MAX30003_COMMON_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


#define PIN_NUM_MOSI 3
#define PIN_NUM_MISO 0
#define PIN_NUM_SCK 1
#define PIN_NUM_CS 10

#define CLK_SPEED_SPI 100*1000

#define MAX3_REG_NO_OP          0x00
#define MAX3_REG_STATUS         0x01
#define MAX3_REG_EN_INT         0x02
#define MAX3_REG_EN_INT2        0x03
#define MAX3_REG_MNGR_INT       0x04
#define MAX3_REG_MNGR_DYN       0x05
#define MAX3_REG_SW_RST         0x08
#define MAX3_REG_SYNCH          0x09
#define MAX3_REG_FIFO_RST       0x0A
#define MAX3_REG_INFO           0x0F
#define MAX3_REG_CNFG_GEN       0x10
#define MAX3_REG_CNFG_CAL       0x12
#define MAX3_REG_CNFG_EMUX      0x14
#define MAX3_REG_CNFG_ECG       0x15
#define MAX3_REG_CNFG_RTOR1     0x1D
#define MAX3_REG_CNFG_RTOR2     0x1E
#define MAX3_REG_ECG_FIFO_BURST 0x20
#define MAX3_REG_ECG_FIFO       0x21
#define MAX3_REG_RTOR           0x25
#define MAX3_REG_NO_OP2         0x7F

//max30003 SPI initialisation
void max3_init();

//7-bit adress
//1-bit RWB, 1=read, 0=write
//24-bit data sent
//24-bit data recieved
uint32_t max3_transaction(uint8_t address, uint8_t rwb, uint32_t data);

//read register at adress
uint32_t max3_read_reg(uint8_t address);

//write data to adress
void max3_write_reg(uint8_t address, uint32_t data);

//verify that INFO register is read correctly
void max3_info_verify();

//write 0 to SW_RST register to initiate reset of max30003
void max3_software_reset();

#endif /*MAX30003_COMMON_H*/