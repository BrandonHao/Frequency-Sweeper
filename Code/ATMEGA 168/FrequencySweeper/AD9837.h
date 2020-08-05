/****************************************************************
Core header file for MiniGen board support

This code is beerware; if you use it, please buy me (or any other
SparkFun employee) a cold beverage next time you run into one of
us at the local.

2 Jan 2014- Mike Hord, SparkFun Electronics

Code developed in Arduino 1.0.5, on an Arduino Pro Mini 5V.

**Updated to Arduino 1.6.4 5/2015**
****************************************************************/
#ifndef AD9837_H
#define AD9837_H

#include <inttypes.h>

#include "Hardware.h"
#include "IO_Macros.h"
#include "SPI.h"

#define AD9837_SPI_MODE SPI_MODE2
#define AD9837_SPI_CLK_DIV SPI_CLK_DIV_16
uint16_t _config_reg;

uint16_t _fsync_pin;

typedef enum {TRIANGLE, SINE, SQUARE, SQUARE_2} Mode;
typedef enum {FREQ0, FREQ1} FreqReg;
typedef enum {PHASE0, PHASE1} PhaseReg;
typedef enum {FULL, COARSE, FINE} FreqAdjustMode;

void dds_reset();
void set_dds_mode(Mode newMode);
void select_freq_reg(FreqReg reg);
void select_phase_reg(PhaseReg reg);
void set_freq_adjust_mode(FreqAdjustMode newMode);
void adjust_phase_shift(PhaseReg reg, uint16_t newPhase);
void adjust_freq_and_mode_32(FreqReg reg, FreqAdjustMode mode, uint32_t newFreq);
void adjust_freq_and_mode_16(FreqReg reg, FreqAdjustMode mode, uint16_t newFreq);
void adjust_freq_32(FreqReg reg, uint32_t newFreq);
void adjust_freq_16(FreqReg reg, uint16_t newFreq);
uint32_t freq_calc(float desiredFrequency);
void _spi_write(uint16_t data);

#endif