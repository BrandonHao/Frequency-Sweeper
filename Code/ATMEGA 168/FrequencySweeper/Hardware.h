/*
 * Hardware.h
 * Hardware defines
 * Created: 2020-08-01 3:03:54 PM
 *  Author: Brandon
 */ 

#ifndef HARDWARE_H_INCLUDED
#define HARDWARE_H_INCLUDED

#define F_CPU 16000000UL

//LCD pins			PORT, PIN
#define LCD_D4		D, 5
#define LCD_D5		D, 6
#define LCD_D6		D, 7
#define LCD_D7		B, 0
#define LCD_RS		D, 2
#define LCD_RW      D, 3
#define LCD_EN		D, 4

//CS pins			PORT, PIN
#define DDS_CS      C, 0
#define ADC_CS      B, 2

//SPI pins			PORT, PIN
#define MOSI        B, 3
#define MISO        B, 4
#define SCLK        B, 5
#endif