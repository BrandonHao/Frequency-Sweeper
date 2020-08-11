/*!
*  @file Adafruit_MCP3008.h
*
*  This is a library for the MCP3008 - 8-Channel 10-Bit ADC with SPI Interface.
*
*  Designed specifically to work with the Adafruit MCP3004.
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
*  MIT license, all text above must be included in any redistribution
*/

//Modified for use in this project by Brandon Hao 2020-08-10

#ifndef MCP3004_H
#define MCP3004_H

#include "SPI.h"
#include "IO_Macros.h"
#include "Hardware.h"

#define MCP3004_SPI_MAX_5V 3600000         ///< SPI MAX Value on 5V pin
#define MCP3004_SPI_MAX_3V 1350000         ///< SPI MAX Value on 3V pin
#define MCP3004_SPI_MAX MCP3004_SPI_MAX_3V ///< SPI MAX Value
#define MCP3004_SPI_MODE SPI_MODE0         ///< SPI MODE
#define MCP3004_SPI_CLK_DIV SPI_CLK_DIV_64


void adc_setup();
uint16_t read_adc(uint8_t channel);
uint16_t _spi_adc(uint8_t channel, uint8_t differential);

#endif