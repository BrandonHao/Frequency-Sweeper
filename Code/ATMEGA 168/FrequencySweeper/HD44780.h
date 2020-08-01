#ifndef HD44780_H_INCLUDED
#define HD44780_H_INCLUDED
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

//----- Headers ------------//
#define F_CPU 16000000UL
#include <inttypes.h>
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "IO_Macros.h"
//--------------------------//

//----- Auxiliary data ---------------------------//

//LCD pins			PORT, PIN
#define LCD_D4		D, 5
#define LCD_D5		D, 6
#define LCD_D6		D, 7
#define LCD_D7		B, 0
#define LCD_RS		D, 2
#define LCD_RW      D, 3
#define LCD_EN		D, 4

//LCD pins			PORT, PIN
#define _LCD_D4		PORTD, 5
#define _LCD_D5		PORTD, 6
#define _LCD_D6		PORTD, 7
#define _LCD_D7		PORTB, 0
#define _LCD_RS		PORTD, 2
#define _LCD_RW     PORTD, 3
#define _LCD_EN		PORTD, 4

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
void SetOutputs();
void SetInputs();
void LCD_Setup();
void LCD_SendCommand(uint8_t Command);
void LCD_SendData(char Character);
void LCD_WaitBusy();
void LCD_BuildChar(char *Data, uint8_t Position);
void LCD_BuildChar_P(const char *Data, uint8_t Position);

void LCD_Clear();
void LCD_ClearLine(uint8_t Line);
void LCD_GotoXY(uint8_t X, uint8_t Y);
Point_t LCD_GetP();
uint8_t LCD_GetX();
uint8_t LCD_GetY();

void LCD_PrintChar(char Character);
void LCD_PrintString(char *Text);
void LCD_PrintString_P(const char *Text);
void LCD_PrintInteger(int32_t Value);
void LCD_PrintDouble(double Value, uint32_t Tens);
//-----------------------------------------------------------------------------//
#endif
