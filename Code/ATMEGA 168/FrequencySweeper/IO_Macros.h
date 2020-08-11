/*
 * IO_Macros.h
 * Macros for pin manipulation
 * Created: 2020-08-01 3:03:54 PM
 *  Author: Brandon
 */ 

#ifndef IO_MACROS_H_INCLUDED
#define IO_MACROS_H_INCLUDED
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
