/*
 * SPI.c
 *
 * Created: 2020-08-01 3:13:32 PM
 *  Author: Brandon
 */

#include "SPI.h"

void spi_init()
{
    SPCR = (1 << SPE | 1 << MSTR);
    spi_set_clk_div(SPI_CLK_DIV_16);
    SPSR = 0;
}

void spi_set_mode(uint8_t spiMode)
{
    SPCR |= (SPCR & ~SPI_MODE_MASK) | spiMode;
}

void spi_set_clk_div(uint8_t spiSpeed)
{
    SPCR |= (SPCR & ~SPI_CLK_DIV_MASK) | spiSpeed;
}

uint8_t spi_transfer(uint8_t data)
{
    SPDR = data;
    while(!(SPSR & (1<<SPIF)));
    
    return SPDR;
}