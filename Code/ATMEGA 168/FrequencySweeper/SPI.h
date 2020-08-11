/*
 * SPI.h
 * SPI code
 * Created: 2020-08-01 3:13:04 PM
 *  Author: Brandon
 */ 

#ifndef SPI_H
#define SPI_H

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

void spi_init();
void spi_set_mode(uint8_t spiMode);
void spi_set_clk_div(uint8_t spiSpeed);
uint8_t spi_transfer(uint8_t data);

#endif