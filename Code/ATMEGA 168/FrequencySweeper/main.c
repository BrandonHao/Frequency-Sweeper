/*
* FrequencySweeper.c
*
* Created: 2020-07-26 5:55:24 PM
* Author : Brandon
*/

#define F_CPU 16000000UL

#include <avr/io.h>
#include "Hardware.h"
#include "IO_Macros.h"
#include "HD44780.h"
#include "AD9837.h"

int main(void)
{
    //Setup
    lcd_setup();
    lcd_clear_line(0);
    _delay_ms(1000);
    //Print
    lcd_goto_xy(0, 0);
    lcd_print_string("Start!");
    _delay_ms(1000);
    lcd_goto_xy(0, 1);
    lcd_print_string("Stop!");
    dds_reset();
    set_dds_mode(SINE);
    uint32_t freq = freq_calc(100000);
    adjust_freq_and_mode_32(FREQ0, FULL, freq);
    while (1);
    
    return 0;
}

