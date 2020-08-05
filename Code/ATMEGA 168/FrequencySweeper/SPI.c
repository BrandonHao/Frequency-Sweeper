/*
 * SPI.c
 *
 * Created: 2020-08-01 3:13:32 PM
 *  Author: Brandon
 */

#include "SPI.h"

int spi_transfer (uint8_t data, uint8_t spiMode, uint8_t clkDiv)
{
    SPCR = ((SPCR & ~SPI_MODE_MASK) | spiMode) | ((SPCR & ~SPI_CLK_DIV_MASK) | clkDiv);
    SPDR = data >> 8;
    while(!(SPSR & (1<<SPIF)));
    SPDR = data;
    
    return SPSR;
}