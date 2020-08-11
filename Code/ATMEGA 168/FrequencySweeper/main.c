/*
* FrequencySweeper.c
* 
* Main execution code
*
* Created: 2020-07-26 5:55:24 PM
* Author : Brandon
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdio.h>
#include "Hardware.h"
#include "IO_Macros.h"
#include "HD44780.h"
#include "AD9837.h"
#include "MCP3004.h"
#include "SPI.h"

void setup(){
    //Setup
    lcd_setup();
    dds_setup();
    adc_setup();
    spi_init();
    
    //Print
    lcd_goto_xy(0, 0);
    lcd_print_string("COIL TUNER:");
    lcd_goto_xy(0, 1);
    dds_reset();
    set_freq_adjust_mode(FULL);
}

uint32_t incrementFreq(uint32_t currFreq){
    currFreq += 10;
    uint32_t freqRegVal = freq_calc(currFreq);
    adjust_freq_32(FREQ0, freqRegVal);
    return currFreq;
}

uint16_t sampleADC(uint8_t sampleCount){
    uint16_t adcSum = 0;
    for(uint8_t i = 0; i < sampleCount; i++){
        adcSum += read_adc(0);
    }
    return adcSum/sampleCount;
}

int main(void){
    setup();
    
    uint32_t baseFreq = 100000;
    uint32_t maxFreq = 150000;
    uint32_t currFreq = baseFreq;
    uint32_t peakFreq = 0;
    uint16_t adcPeak = 0;
    
    set_dds_mode(SINE);
    select_freq_reg(FREQ0);
    uint32_t freqRegVal = freq_calc(100000);
    adjust_freq_and_mode_32(FREQ0, FULL, freqRegVal);
    while (1){
        currFreq = incrementFreq(currFreq);
        _delay_us(10);
        uint16_t adcCurrent = sampleADC(10);
        if(adcCurrent > adcPeak){
            adcPeak = adcCurrent;
            peakFreq = currFreq;
        }
        if(currFreq == maxFreq){
            lcd_clear_line(1);
            if(peakFreq > 110100){
                char outputText[20];
                int peakFreqInt = peakFreq/1000;
                int peakFreqDec = (peakFreq/100 % 10);
                sprintf(outputText, "Peak: %d.%dKHz", peakFreqInt, peakFreqDec);
                lcd_goto_xy(0, 1);
                lcd_print_string(outputText);
            }
            peakFreq = baseFreq;
            adcPeak = 0;
            currFreq = baseFreq;
        }
    }
    
    return 0;
}

