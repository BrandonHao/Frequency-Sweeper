/*
||
||  Filename:	 		HD44780.h
||  Title: 			    HD44780 Driver
||  Author: 			Efthymios Koktsidis
||	Email:				efthymios.ks@gmail.com
||  Compiler:		 	AVR-GCC
||	Description:
||	This library can drive HD44780 based LCD.
||	The LCD is driven exclusively in 4-bit mode.
||
*/

//Modified for use in this project by Brandon Hao 2020-08-10

#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED

//----- Headers ------------//
#include "IO_Macros.h"
#include "Hardware.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
//--------------------------//

//----- Auxiliary data ---------------------------//

#define __LCD_Pulse_us					1
#define __LCD_Delay_1					20
#define __LCD_Delay_2					10
#define __LCD_Delay_3					1
#define __LCD_Delay_4					1

#define __LCD_CMD_ClearDisplay			0x01
#define __LCD_CMD_EntryModeSet			0x04
#define __LCD_CMD_DisplayControl		0x08
#define __LCD_CMD_FunctionSet			0x20
#define __LCD_CMD_SetCGRAMAddress		0x40
#define __LCD_CMD_SetDDRAMAddress		0x80

#define __LCD_CMD_EntryIncrement		0x02
#define __LCD_CMD_EntryNoShift			0x00

#define __LCD_CMD_DisplayOn				0x04
#define __LCD_CMD_CursorOff				0x00
#define __LCD_CMD_BlinkOff				0x00

#define __LCD_CMD_8BitMode				0x10
#define __LCD_CMD_4BitMode				0x00
#define __LCD_CMD_2Line					0x08
#define __LCD_CMD_5x8Dots				0x00

#define __LCD_BusyFlag					7

typedef struct
{
	uint8_t X,Y;
}Point_t;
//------------------------------------------------//

//----- Prototypes ------------------------------------------------------------//
void lcd_setup();
void _lcd_send_command(uint8_t Command);
void _lcd_send_data(char Character);
void _lcd_wait_busy();
void _lcd_build_char(char *Data, uint8_t Position);
void _lcd_build_char_p(const char *Data, uint8_t Position);

void lcd_clear();
void lcd_clear_line(uint8_t Line);
void lcd_goto_xy(uint8_t X, uint8_t Y);
Point_t _lcd_get_p();
uint8_t _lcd_get_x();
uint8_t _lcd_get_y();

void lcd_print_char(char Character);
void lcd_print_string(char *Text);
void lcd_print_string_p(const char *Text);
void lcd_print_integer(int32_t Value);
void lcd_print_double(double Value, uint32_t Tens);
//-----------------------------------------------------------------------------//
#endif
