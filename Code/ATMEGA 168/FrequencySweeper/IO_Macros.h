#ifndef IO_MACROS_H_INCLUDED
#define IO_MACROS_H_INCLUDED
/*
||
||  Filename:	 		IO_Macros.h
||  Title: 			    IO manipulation macros
||  Author: 			Efthymios Koktsidis
||	Email:				efthymios.ks@gmail.com
||  Compiler:		 	AVR-GCC
||	Description:		This library contains macros for 
||						easy port manipulation (similar 
||						to Arduino).
||
||	Demo:
|| 1.	#define LED		A, 0		|| 6. 	PinModeToggle(BUTTON);
|| 2.	#define BUTTON	A, 1		|| 7. 	DigitalWrite(LED, LOW);
|| 3.								|| 8. 	DigitalWrite(LED, HIGH);
|| 4. 	PinMode(BUTTON, OUTPUT);	|| 9. 	DigitalLevelToggle(LED);
|| 5. 	PinMode(LED, OUTPUT);		||10.	int a = DigitalRead(BUTTON);
||
*/
#include <avr/io.h>

//----- I/O Macros -----
#define _SET_BIT(TYPE, ADDRESS, BIT)    (TYPE##ADDRESS |= (1<<BIT))
#define _CLEAR_BIT(TYPE, ADDRESS, BIT) (TYPE##ADDRESS &= ~(1<<BIT))
#define _FLIP_BIT(TYPE, ADDRESS, BIT)   (TYPE##ADDRESS ^= (1<<BIT))
#define _READ_BIT(TYPE, ADDRESS, BIT)  (TYPE##ADDRESS & (1UL<<BIT) ? 1 : 0)

#define SET_BIT(pin)    _SET_BIT(PORT, pin)
#define CLEAR_BIT(pin)  _CLEAR_BIT(PORT, pin)
#define FLIP_BIT(pin)   _FLIP_BIT(PORT, pin)
#define READ_BIT(pin)  _READ_BIT(PIN, pin)
#define SET_OUTPUT(pin) _SET_BIT(DDR, pin)
#define SET_INPUT(pin) _CLEAR_BIT(DDR, pin)
//------------------
#endif
