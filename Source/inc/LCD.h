/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

LCD code for use with the Hitachi HD44780
*/

#ifndef LCD_H
#define LCD_H

#include "delay.h"

/*******************************
 * LCD pin connections to PORTD
 *******************************
 */
 
//PD5 is skipped because it is connected to an LED.
//Enabling it turns the LED on, which wastes power.
#define RS 7
#define RW 4
#define EN 6

#define DB7 3
#define DB6 2
#define DB5 1
#define DB4 0

// Universal wait time between data transfers to the LCD
#define waitms 1

/*******************************
 * FUNCTION PROTOTYPES
 *******************************
 */

void LCD_port_init(void);
void LCD_init(void);
void LCD_write(unsigned char data);
void LCD_write_string(char string[]);
void LCD_place_cursor(unsigned int line);
void send_CMD(int value1, int value2);
void send_char(int value1, int value2);
void LCD_write_int(int num, int width);
void LCD_write_float(float num, int width, int p);
void LCD_write_double(double num, int width, int p);
void LCD_clear_display(void);
void waitForLCDReady(void);
void send_CMD_BF(int value1, int value2);
void minimalDelay(void);

#endif

