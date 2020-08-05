/*
 * SPI.h
 *
 * Created: 2020-08-01 3:13:04 PM
 *  Author: Brandon
 */ 

#include <avr/io.h>

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C
#define SPI_CLK_DIV_4 0x00
#define SPI_CLK_DIV_16 0x01
#define SPI_CLK_DIV_64 0x02
#define SPI_CLK_DIV_128 0x03
#define SPI_CLK_DIV_MASK 0x03
#define SPI_MODE_MASK 0x0C

int spi_transfer(uint8_t data, uint8_t spiMode, uint8_t clkDiv);