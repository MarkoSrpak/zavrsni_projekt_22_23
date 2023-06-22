#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "driver/gpio.h"

#include "driver/spi_common.h"
#include "driver/spi_master.h"
#include "hal/spi_types.h"

#include "maxChip.h"

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

uint32_t max3_transaction(uint8_t adress, uint8_t rwb, uint32_t data){
    uint8_t transmit_buffer[4];
    memset(&transmit_buffer, 0, sizeof(transmit_buffer));
    transmit_buffer[0] = (adress << 1) | (rwb & 0x01);
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

uint32_t max3_read_reg(uint8_t adress){
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    if(adress == MAX3_REG_NO_OP ||
        adress == MAX3_REG_STATUS ||
        adress == MAX3_REG_EN_INT || 
        adress == MAX3_REG_EN_INT2 ||
        adress == MAX3_REG_MNGR_INT ||
        adress == MAX3_REG_MNGR_DYN ||
        adress == MAX3_REG_INFO ||
        adress == MAX3_REG_CNFG_GEN ||
        adress == MAX3_REG_CNFG_CAL ||
        adress == MAX3_REG_CNFG_EMUX ||
        adress == MAX3_REG_CNFG_ECG ||
        adress == MAX3_REG_CNFG_RTOR1 ||
        adress == MAX3_REG_CNFG_RTOR2 ||
        adress == MAX3_REG_ECG_FIFO ||
        adress == MAX3_REG_RTOR ||
        adress == MAX3_REG_NO_OP2){
            ret = ESP_OK;
        }
    ESP_ERROR_CHECK(ret);
    return max3_transaction(adress, 1, 0x00);
}

void max3_write_reg(uint8_t adress, uint32_t data){
    esp_err_t ret = ESP_ERR_INVALID_ARG;
    if(adress == MAX3_REG_NO_OP ||
        adress == MAX3_REG_EN_INT || 
        adress == MAX3_REG_EN_INT2 ||
        adress == MAX3_REG_MNGR_INT ||
        adress == MAX3_REG_MNGR_DYN ||
        adress == MAX3_REG_SW_RST ||
        adress == MAX3_REG_SYNCH ||
        adress == MAX3_REG_FIFO_RST ||
        adress == MAX3_REG_CNFG_GEN ||
        adress == MAX3_REG_CNFG_CAL ||
        adress == MAX3_REG_CNFG_EMUX ||
        adress == MAX3_REG_CNFG_ECG ||
        adress == MAX3_REG_CNFG_RTOR1 ||
        adress == MAX3_REG_CNFG_RTOR2 ||
        adress == MAX3_REG_NO_OP2){
            ret = ESP_OK;
        }
    ESP_ERROR_CHECK(ret);
    max3_transaction(adress, 0, data);
}

void max3_info_verify(){
    max3_read_reg(MAX3_REG_NO_OP);
    uint32_t info_reg = max3_read_reg(MAX3_REG_INFO);
    esp_err_t ret = ESP_ERR_INVALID_RESPONSE;
    if(info_reg >> 20 == 0x5){ 
        ret = ESP_OK;
        printf("MAX30003 initialised and working\n");
    }
    ESP_ERROR_CHECK(ret);
}

void max3_software_reset(){
    max3_write_reg(MAX3_REG_SW_RST, 0x00);
}

void max3_synchronize(){
    max3_write_reg(MAX3_REG_SYNCH, 0x00);
}

void max3_FIFO_reset(){
    max3_write_reg(MAX3_REG_FIFO_RST, 0x00);
}

void max3_ECG_enable(uint8_t enable){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_GEN);
    if(enable){
        max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg | (1 << 19));
    }
    else{
        max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg & ~(1 << 19));
    }
}

void max3_FMSTR_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_GEN);
    cnfg_reg &= ~(0x300000);
    cnfg_reg |= (mode & 0x3) << 20;
    max3_write_reg(MAX3_REG_CNFG_GEN, cnfg_reg);
}

void max3_ECG_rate_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0xC00000);
    cnfg_reg |= (mode & 0x3) << 22;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_gain_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x30000);
    cnfg_reg |= (mode & 0x3) << 16;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_digital_high_pass_filter_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x4000);
    cnfg_reg |= (mode & 0x1) << 14;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

void max3_ECG_digital_low_pass_filter_set(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_ECG);
    cnfg_reg &= ~(0x3000);
    cnfg_reg |= (mode & 0x3) << 12;
    max3_write_reg(MAX3_REG_CNFG_ECG, cnfg_reg);
}

uint32_t max3_ECG_read(){
    uint32_t ECG_data = max3_read_reg(MAX3_REG_ECG_FIFO);
    uint32_t ECG_sample = (ECG_data & 0xFFFFC0) >> 5;
    if(ECG_sample & 0x20000) ECG_sample |= 0xFFFC0000; //ako je 18 bitni negativan, pretvori u 32 bitni negativan
    uint8_t ETAG = (ECG_data & 0x38) >> 3;
 //   printf("ETAG %d\n", ETAG);
    if(ETAG == 0){ //valid sample
        printf("%ld\n", ECG_sample);
        return 1;
    }
    if(ETAG == 1){ //fast - not valid data, valid time
    }
    if(ETAG == 2){ //valid, EOF
        printf("%ld\n", ECG_sample);
        return 0;
    }
    if(ETAG == 3){ //fast - not valid data, valid time, EOF

    }
    //ETAG 4 and 5 are not used
    if(ETAG == 6){ //FIFO is empty
        return 0;
    }
    if(ETAG == 7){ //FIFO is in overflow
        max3_FIFO_reset();
        return 0;
    }
    return 0;
}

void max3_RTOR_enable(uint8_t enable){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_CNFG_RTOR1);
    if(enable){
        max3_write_reg(MAX3_REG_CNFG_RTOR1, cnfg_reg | (1 << 15));
    }
    else{
        max3_write_reg(MAX3_REG_CNFG_RTOR1, cnfg_reg & ~(1 << 15));
    }
}

uint32_t max3_RTOR_read(){
    return max3_read_reg(MAX3_REG_RTOR) >> 10;
}

void max3_RTOR_interrupt_behaviour(uint8_t mode){
    uint32_t cnfg_reg = max3_read_reg(MAX3_REG_MNGR_INT);
    cnfg_reg &= ~(0x30);
    cnfg_reg |= (mode & 0x3) << 4;
    max3_write_reg(MAX3_REG_MNGR_INT, cnfg_reg);
}