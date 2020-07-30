/*
* FrequencySweeper.c
*
* Created: 2020-07-26 5:55:24 PM
* Author : Brandon
*/

#define F_CPU 16000000UL

#ifdef __cplusplus
extern "C"
{
    #endif
    
    #include <avr/io.h>
    #include "HD44780.h"

    #ifdef __cplusplus
}
#endif

#include "AD9837.hpp"

int main(void)
{
    //Setup
    LCD_Setup();
    LCD_ClearLine(0);
    _delay_ms(1000);
    //Print
    LCD_GotoXY(0, 0);
    LCD_PrintString("Start!");
    _delay_ms(3000);
    LCD_GotoXY(0, 1);
    LCD_PrintString("Stop!");
    
    while (1);
    
    return 0;
}

