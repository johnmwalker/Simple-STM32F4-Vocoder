#ifndef andyLCD_H
#define andyLCD_H

#include "stm32f4xx.h"
#include <string.h>
#include <stdio.h>
#include "delay.h"
	
	/********************************  LCD pins connections to PORTD********************************/
#define RS 7
#define EN 6
#define DB7 3
#define DB6 2
#define DB5 1
#define DB4 0
/********************************  FUNCTION PROTOTYPES********************************/
// Delay Functions
void tim6_delay(void);
void delay(int ms);
// LCD related functions
void LCD_port_init(void);
void LCD_init(void);
void LCD_write(char data[]);
void place_lcd_cursor(int lineno);
void reverse(char data[]);
void oneatatime(char data[]);
void vowel(char data[]);
void clear(void);
void number(int num);
	
#endif

