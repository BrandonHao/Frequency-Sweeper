/*!
*  @file Adafruit_MCP3008.cpp
*
*  @mainpage Adafruit MCP3008 - 8-Channel 10-Bit ADC
*
*  @section intro_sec Introduction
*
*  This is a library for the MCP3008 - 8-Channel 10-Bit ADC.
*
*  Designed specifically to work with the Adafruit MCP3008.
*
*  Pick one up today in the adafruit shop!
*  ------> https://www.adafruit.com/product/856
*
*  These breakout use SPI to communicate, 4 ping are required.
*
*  Adafruit invests time and resources providing this open source code,
*  please support Adafruit andopen-source hardware by purchasing products
*  from Adafruit!
*
*  @section license License
*
*  MIT license, all text above must be included in any redistribution
*/

//Modified for use in this project by Brandon Hao 2020-08-10

#include "MCP3004.h"

void adc_setup() {
    SET_OUTPUT(DDS_CS);
    SET_OUTPUT(ADC_CS);
    SET_OUTPUT(MOSI);
    SET_OUTPUT(SCLK);
    SET_INPUT(MISO);
    SET_BIT(DDS_CS);
    SET_BIT(ADC_CS);
}

/*!
*    @brief  Read single ended ADC channel.
*    @param  channel
*            channel number
*    @return -1 if channel < 0 or channel > 7, otherwise ADC (int)
*/
uint16_t read_adc(uint8_t channel) {
    if ((channel < 0) || (channel > 3)) {
        return -1;
    }

    return _spi_adc(channel, 0);
}

// SPI transfer for ADC read
uint16_t _spi_adc(uint8_t channel, uint8_t differential) {
    uint8_t command;

    differential = !differential;

    command = ((0x01 << 7) |             // start bit
               (differential << 6) |          // single or differential
               ((channel & 0x03) << 3)); // channel number

    uint8_t b0, b1, b2;

    spi_set_mode(MCP3004_SPI_MODE);

    CLEAR_BIT(ADC_CS);

    b0 = spi_transfer(command);
    b1 = spi_transfer(0x00);
    b2 = spi_transfer(0x00);

    SET_BIT(ADC_CS);

    return 0x3FF & ((b0 & 0x01) << 9 | (b1 & 0xFF) << 1 | (b2 & 0x80) >> 7);
}
