/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

LCD code for use with the Hitachi HD44780
*/

#include "stm32f4xx.h" 
#include <string.h>
#include <stdio.h>
#include "LCD.h"

/* Original main function kept for reference */
/*int main(void) 
{
	// String which will be used for demoing
	char string[] = "Hello ENGR 331";
	
	// Width and precision for displaying numbers 
	int width = 3;
	int p = 3;
	
	// Perform necessary initializations
	LCD_port_init();
	LCD_init();
	
	//Task 3
	//Part 1: Display a message on the LCD all at once
	LCD_place_cursor(0);
	LCD_write_string(string);
	delay(1000);
	
	//Part 2: Reverse the message and display it on the LCD
	send_CMD(0x0, 1<<DB4); // Clear display
	LCD_place_cursor(0);
	reverse_string(string);
	LCD_write_string(string);
	delay(1000);
	
	//Part 3: Display one character after the other
	//(Could write a function to do this if necessary)
	send_CMD(0x0, 1<<DB4); // Clear display
	LCD_place_cursor(0);
	LCD_write('H');
	delay(500);
	LCD_write('e');
	delay(500);
	LCD_write('l');
	delay(500);
	LCD_write('l');
	delay(500);
	LCD_write('o');
	delay(1000);
	
	//Part 4: Capitalize every vowel and then display the modified messsage
	send_CMD(0x0, 1<<DB4); // Clear display
	LCD_place_cursor(0);
	reverse_string(string); // Re-reverse the string back to normal
	capitalize_vowels(string);
	LCD_write_string(string);
	delay(1000);
	
	//Test int and float functions
	send_CMD(0x0, 1<<DB4); // Clear display
	LCD_place_cursor(0);
	LCD_write_int(331, width);
	delay(1000);
	
	send_CMD(0x0, 1<<DB4); // Clear display
	LCD_place_cursor(0);
	LCD_write_float(1.2345678910, width, p);
	delay(1000);

return 0; */

/*******************************
 * LCD_port_init()
 * Inputs: NONE
 * Outputs: NONE
 * Port Initialization
 * Set appropriate pins as digital input/outputs
 *******************************
 */
void LCD_port_init()
{
//STEP 1: Enable GPIOD in RCC AHB1ENR register
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

//STEP 2: Set MODER of GPIOD Pins 0-6 as outputs
	GPIOD->MODER |= (GPIO_MODER_MODER0_0 |
									 GPIO_MODER_MODER1_0 |
									 GPIO_MODER_MODER2_0 |
									 GPIO_MODER_MODER3_0 |
									 GPIO_MODER_MODER4_0 |
									 GPIO_MODER_MODER7_0 |
									 GPIO_MODER_MODER6_0);

}

/*******************************
 * LCD_init()
 * Inputs: NONE
 * Outputs: NONE
 * LCD Initialization
 *******************************
 */
void LCD_init()
{
	int i;
	
// STEP 1: Wait for 100ms for power-on-reset to take effect
	delay_nms(100);

// STEP 2: Set RS pin LOW to send instructions
	GPIOD->ODR &= ~(1<<RS | 1<<RW); 

// Send instructions using following format:
// Set EN=HIGH; Send 4-bit instruction; Set EN=low; delay 20ms;

// STEP 3a-3d: Set 4-bit mode (takes a total of 4 steps)
// Send command 3 times
	for(i=0;i<3;i++)
	{
		send_CMD((1<<DB5) | (1<<DB4), -1);
		delay_nms(5);
	}

	send_CMD(1<<DB5, -1);
	
// STEP 4: Set 2 line display -- treats 16 char as 2 lines
	send_CMD(1<<DB5, (1<<DB7) | (1<<DB6));
	
// STEP 5: Set DISPLAY to OFF
	send_CMD(0x0, 1<<DB7);
	
// STEP 6: CLEAR DISPLAY
	send_CMD(0x0, 1<<DB4);
	
// STEP 7: SET ENTRY MODE - Auto increment; no scrolling
	send_CMD(0x0, (1<<DB6) | (1<<DB5));
	
// STEP 8: Set Display to ON with Cursor and Blink.
	send_CMD(0x0, 0xF);
}

/*******************************
 * LCD_place_cursor()
 * Inputs: unsigned integer
 * Outputs: NONE
 * sets Cursor position to
 * Line 1 if line = 0(hex address 0x80)
 * or Line 2 if line = 1 (hex addres 0xC0)
 *
 *******************************
 */

void LCD_place_cursor(unsigned int line)
{
	if (line == 0)
	{
		send_CMD_BF(1<<DB7,0x0);
	}else
	{
		send_CMD_BF((1<<DB7) | (1<<DB6), 0x0);
	}
	waitForLCDReady();
}

/*******************************
 * LCD_write()
 * Inputs: unsigned character data (8-bit)
 * Outputs: NONE
 * writes the character to LCD.
 *
 *******************************
 */
void LCD_write(unsigned char data)
{
	unsigned char value;
	
	//Tell the LCD we're sending a character
	GPIOD->ODR |= (1<<RS);
	
	//Right shift 4 times leaving only the 4 MSBs
	value = data >> 4;
	
	//Send the value and latch the enable
	GPIOD->ODR &=~0x0F; // Clear DBs to 0
	GPIOD->ODR |= (1<<EN);
	GPIOD->ODR |= value;
	minimalDelay();

	//delay_nms(1);
	GPIOD->ODR &=~(1<<EN);
	//waitForLCDReady();
	
	//delay(waitms);
	
	//Now do the same with the 4 LSBs
	value = data & 0xF;

	GPIOD->ODR &=~0x0F; // Clear DBs to 0
	GPIOD->ODR |= (1<<EN);
	GPIOD->ODR |= value;
	minimalDelay();
	GPIOD->ODR &=~(1<<EN);
	
	//Always delay between commands
	waitForLCDReady();
}

/*******************************
 * LCD_write_string()
 * Inputs: array of char (string)
 * Outputs: NONE
 * Writes the string to the LCD.
 * Assumes that the cursor was originally placed at the first line
 * of the LCD before writing and moves the cursor to the second
 * line if necessary.
 *******************************
 */
void LCD_write_string(char string[])
{
	int i;
	
	// Iterate through the array and write each char
	for(i = 0; i < strlen(string); i++)
	{
		// If we fill up line 1, move to line 2
		if(i == 8)
		{
			LCD_place_cursor(1);
		}
		LCD_write(string[i]);
	}
}

/*******************************
 * send_CMD()
 * Inputs: int value1, int value2
 * Outputs: NONE
 * Sends either a 4-bit or 8-bit command to the LCD.
 * Since the LCD is assumed to be in 4-bit mode, to
 * send an 8-bit command, the command must be split
 * in half, with each half corresponding to value1 
 * and value2.
 * If the command is only 4 bits, place -1 as value2.
 *******************************
 */
void send_CMD(int value1, int value2)
{
	// Tell the LCD we're sending a command
	GPIOD->ODR &= ~(1<<RS); 
	
	// Send the first half and latch the enable bit
	GPIOD->ODR &=~0xFF; // Clear DBs to 0
	GPIOD->ODR |= (1<<EN);
	GPIOD->ODR |= value1;
	delay_nms(1);
	GPIOD->ODR &=~(1<<EN);
	
	//delay(waitms);
	
	// If there's a second half, repeat the process
	if(value2 != -1)
	{
		GPIOD->ODR &=~0xFF; // Clear DBs to 0
		GPIOD->ODR |= (1<<EN);
		GPIOD->ODR |= value2;
		delay_nms(1);
		GPIOD->ODR &=~(1<<EN);
	}
	
	// Always delay after sending data to the LCD
	delay_nms(1);

}
void send_CMD_BF(int value1, int value2)
{
	// Tell the LCD we're sending a command
	GPIOD->ODR &= ~(1<<RS); 
	
	// Send the first half and latch the enable bit
	GPIOD->ODR &=~0xFF; // Clear DBs to 0
	GPIOD->ODR |= (1<<EN);
	GPIOD->ODR |= value1;
	minimalDelay();
	GPIOD->ODR &=~(1<<EN);
	
	//delay(waitms);
	
	// If there's a second half, repeat the process
	if(value2 != -1)
	{
		GPIOD->ODR &=~0xFF; // Clear DBs to 0
		GPIOD->ODR |= (1<<EN);
		GPIOD->ODR |= value2;
		minimalDelay();
		GPIOD->ODR &=~(1<<EN);
	}
	
	// Always delay after sending data to the LCD
	waitForLCDReady();
}

// Writes a decimal value to LCD
void LCD_write_int(int num, int width)
{
	char str[17];
	sprintf(str, "%*d", width, num);
	LCD_write_string(str);
}

void LCD_clear_display(void)
{
	send_CMD(0x0, 1<<DB4);
}

// Writes a float value LCD
// width is the minimum width of the value. Blank spaces
// will be appended if the value is smaller than width.
// p is the precision of the float (number of decimal places).
void LCD_write_float(float num, int width, int p)
{
	char str[17];
	sprintf(str, "%*.*f", width, p, num);
	//sprintf(str, "%f", num);

	LCD_write_string(str);
}

// Writes a double value LCD
// width is the minimum width of the value. Blank spaces
// will be appended if the value is smaller than width.
// p is the precision of the float (number of decimal places).
void LCD_write_double(double num, int width, int p)
{
	char str[17];
	sprintf(str, "%*.*f", width, p, num);
	LCD_write_string(str);
}

/* Wait for DB7 to go to zero to signify that the LCD is no longer busy
*/
void waitForLCDReady(void)
{
	GPIOD->ODR &= ~(0xFF);
		//Clear RS and set RW to 1 to read busy flag
	GPIOD->ODR |= (1<<RW);

	//Set DB pins to Input mode (though we only really care about DB7)
	GPIOD->MODER &= ~(GPIO_MODER_MODER0_0 |
								 GPIO_MODER_MODER1_0 |
								 GPIO_MODER_MODER2_0 |
								 GPIO_MODER_MODER3_0);
	
	int done=0;
	
	//Wait for DB7 to go to zero
	//I realize that this while loop could just be written while(GPIOD->IDR * (1<<DB7)){}
	//but before I wrote it this way, it would occasionally get stuck. Now it doesn't.
	while(done==0)
	{
		if(GPIOD->IDR & (1<<DB7))
		{
			done=1;
		}
	}
	
	//Set RW back to zero
	GPIOD->ODR &= ~(1<<RW);
	
	//Set pins back to normal
	LCD_port_init();
}
/* Note that this function only seems necessary due to the faster-than-usual
168MHz clock used for audio-based applications. Normally, the latching to
the LCD is fine without any additional delay
*/
void minimalDelay(void)
{
	delay_nus(100);
}
