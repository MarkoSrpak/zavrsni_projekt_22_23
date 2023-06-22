#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/gpio.h"

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "hal/spi_types.h"

#include "max30003_common.h"

//GND -> GND
//VCC -> 3.3V
//SDI(MOSI) -> GPIO3
//SDO(MISO) -> GPIO0
//SCK(CLK) -> GPIO1
//CS(SS) -> GPIO10

static spi_bus_config_t spi_bus_config = {
    .mosi_io_num = PIN_NUM_MOSI,
    .miso_io_num = PIN_NUM_MISO,
    .sclk_io_num = PIN_NUM_SCK,
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
    .max_transfer_sz = 4,
};

static spi_device_interface_config_t spi_device_interface_config = {
    .command_bits = 0,
    .address_bits = 0,
    .dummy_bits = 0,
    .mode = 0,
//    .duty_cycle_pos = 128, //128 od 256 je 50%/50% duty cycle, ovo je i default vrijednost
    //.cs_ena_pretrans, //prazni ciklusi prije prvog, nebitno
    //.cs_ena_posttrans, //prazni ciklusi nakon zadnjeg, nebitno
    .clock_speed_hz = CLK_SPEED_SPI, //100kHz
   // .input_delay_ns = 10, //mozda treba, nisam siguran
    .spics_io_num = PIN_NUM_CS,
    //.flags,// nebitno
    .queue_size = 1, //nebitno
    //.pre_cb, //nebitno
    //.post_cb, //nebitno
};

static spi_device_handle_t spi_device_handle;

void max3_init(){
    esp_err_t ret;
    ret = spi_bus_initialize(SPI2_HOST, &spi_bus_config, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    ret = spi_bus_add_device(SPI2_HOST, &spi_device_interface_config, &spi_device_handle); 
    ESP_ERROR_CHECK(ret);
    gpio_set_level(PIN_NUM_CS, 1);
}


uint32_t max3_transaction(uint8_t address, uint8_t rwb, uint32_t data){
    uint8_t transmit_buffer[4];
    memset(&transmit_buffer, 0, sizeof(transmit_buffer));
    transmit_buffer[0] = (address << 1) | (rwb & 0x01);
    transmit_buffer[1] = (data >> 16) & 0xFF;
    transmit_buffer[2] = (data >> 8) & 0xFF;
    transmit_buffer[3] = data & 0xFF;
    
    uint8_t recieve_buffer[4];
    memset(&recieve_buffer, 0, sizeof(recieve_buffer));
    spi_transaction_t spi_transaction = {
     //   .flags = SPI_TRANS_USE_RXDATA,
        .length = 32,
        .rxlength = 32,
        .tx_buffer = &transmit_buffer,
        .rx_buffer = &recieve_buffer
    };
    esp_err_t ret;
    ret = spi_device_polling_transmit(spi_device_handle, &spi_transaction);
    ESP_ERROR_CHECK(ret);

    uint32_t recieved_data = 0;
    recieved_data = (recieve_buffer[0] << 24) | (recieve_buffer[1] << 16) | (recieve_buffer[2] << 8) | recieve_buffer[3];

    return recieved_data;
}


uint32_t max3_read_reg(uint8_t address){
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    if(address == MAX3_REG_NO_OP         ||
        address == MAX3_REG_STATUS       ||
        address == MAX3_REG_EN_INT       ||
        address == MAX3_REG_EN_INT2      ||
        address == MAX3_REG_MNGR_INT     ||
        address == MAX3_REG_MNGR_DYN     ||
        address == MAX3_REG_INFO         ||
        address == MAX3_REG_CNFG_GEN     ||
        address == MAX3_REG_CNFG_CAL     ||
        address == MAX3_REG_CNFG_EMUX    ||
        address == MAX3_REG_CNFG_ECG     ||
        address == MAX3_REG_CNFG_RTOR1   ||
        address == MAX3_REG_CNFG_RTOR2   ||
        address == MAX3_REG_ECG_FIFO     ||
        address == MAX3_REG_RTOR         ||
        address == MAX3_REG_NO_OP2){
            ret = ESP_OK;
        }
    ESP_ERROR_CHECK(ret);
    return max3_transaction(address, 1, 0x00);
}


void max3_write_reg(uint8_t address, uint32_t data){
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    if(address == MAX3_REG_NO_OP         ||
        address == MAX3_REG_EN_INT       ||
        address == MAX3_REG_EN_INT2      ||
        address == MAX3_REG_MNGR_INT     ||
        address == MAX3_REG_MNGR_DYN     ||
        address == MAX3_REG_SW_RST       ||
        address == MAX3_REG_SYNCH        ||
        address == MAX3_REG_FIFO_RST     ||
        address == MAX3_REG_CNFG_GEN     ||
        address == MAX3_REG_CNFG_CAL     ||
        address == MAX3_REG_CNFG_EMUX    ||
        address == MAX3_REG_CNFG_ECG     ||
        address == MAX3_REG_CNFG_RTOR1   ||
        address == MAX3_REG_CNFG_RTOR2   ||
        address == MAX3_REG_NO_OP2){
            ret = ESP_OK;
        }
    ESP_ERROR_CHECK(ret);
    max3_transaction(address, 0, data);
}


uint32_t max3_read_reg_bits(uint8_t address, uint8_t low_bit, uint8_t high_bit){
    esp_err_t ret = ESP_OK;
    if(low_bit >= 24){
        ret = ESP_ERR_INVALID_ARG;
    }
    if(high_bit >= 24){
        ret = ESP_ERR_INVALID_ARG;
    }
    if(low_bit > high_bit){
        ret = ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(ret);
    uint32_t reg_bits = max3_read_reg(address);
    reg_bits = reg_bits << (31 - high_bit);
    reg_bits = reg_bits >> (31 - high_bit + low_bit);
    return reg_bits;
}


void max3_write_reg_bits(uint8_t address, uint8_t low_bit, uint8_t high_bit, uint32_t data){
    esp_err_t ret = ESP_OK;
    if(low_bit >= 24){
        ret = ESP_ERR_INVALID_ARG;
    }
    if(high_bit >= 24){
        ret = ESP_ERR_INVALID_ARG;
    }
    if(low_bit > high_bit){
        ret = ESP_ERR_INVALID_ARG;
    }
    if(1 << (high_bit - low_bit + 1) < data){
        ret = ESP_ERR_INVALID_ARG;
    }
    ESP_ERROR_CHECK(ret);
    uint32_t reg_bits = max3_read_reg(address);
    uint32_t bits_mask = 0xFFFFFFFF;
    bits_mask = bits_mask << (31 - high_bit);
    bits_mask = bits_mask >> (31 - high_bit + low_bit);
    bits_mask = bits_mask << low_bit;
    reg_bits = reg_bits & ~bits_mask;
    reg_bits = reg_bits | (data << low_bit);
    max3_write_reg(address, reg_bits);
    return;
}


void max3_info_verify(){
    max3_read_reg(MAX3_REG_NO_OP);
    uint32_t info_bits = max3_read_reg_bits(MAX3_REG_INFO, 20, 23);
    esp_err_t ret = ESP_ERR_INVALID_RESPONSE;
    if(info_bits == 0x5){
        ret = ESP_OK;
        printf("MAX30003 initialised and working\n");
    }
    ESP_ERROR_CHECK(ret);
}

void max3_software_reset(){
    max3_write_reg(MAX3_REG_SW_RST, 0x00);
}
