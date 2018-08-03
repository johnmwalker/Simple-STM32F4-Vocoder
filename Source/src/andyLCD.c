/* Lab 4 - ENGR331
  *  The goal of this lab is help you figure out
	*  (on your own) how to interface to a peripheral
  *  that you have never used before based on the
  *  documentation you find (either datasheet or other
	*  available sources/examples online).
	*  ENJOY!!! (and I really mean it!)*/

	#include "andyLCD.h"

	// END Functions
	/********************************  START OF ACTUAL CODE********************************/
	/*******************************
		*  main()
		*  Inputs: NONE
		*  Outputs: NONE
		*  Main function. Goals:
		*  Initialize LCD Port
		*  Initialize LCD
		*  Place Cursor on Line 1
		*  Write character string to Line 1
		*  Place Cursor on Line 2
		*  Write character string to Line 2
	********************************/
	/*
	int main(void) {
		// enter your code here.
		
		LCD_port_init();
		delay(100);
		LCD_init();
		delay(100);
		place_lcd_cursor(1);
		char string[100] = "Hello ENGR331";
		LCD_write(string);
		delay(1000);
		clear();
		reverse(string);
		delay(1000);
		clear();
		oneatatime(string);
		delay(1000);
		clear();
		vowel(string);
		delay(1000);
		clear();
		number(167984);
		return 0;}
	*/
	/*******************************
		*  tim6_delay(void)
		*  Inputs: NONE
		*  Outputs: NONE
		*  Based on PSC=0 and ARR=21000;
		*  we get delay of approximately 1.33ms
	********************************/
void tim6_delay(void){
	// enable APB1 bus clock
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
	//TIM6 prescaler set at default to 0 for now
	TIM6->PSC=0; // prescalar
	TIM6->ARR = 21000;  //auto reload register
	TIM6->CNT=0;   //clear counter register
	TIM6->CR1|=TIM_CR1_CEN;//WHEN COUNTER IS DONE THE TIM6_SR REG UIF FLAG IS SET
	while(TIM6->SR==0);
	TIM6->SR=0; }//CLEAR uIF FLAG}
	/*******************************
		*  delay(int ms)
		*  Inputs: delay in milliseconds
		*  Outputs: NONE
		*  An approximate delay because
		*  call of tim6_delay() creates about 1.33ms
	********************************/
void delay(int ms){
	delay_nms(ms);
}
	/*******************************
		*  LCD_port_init()
		*  Inputs: NONE
		*  Outputs: NONE
		*  Port Initialization
		*  Refer to the #define statements at top to
		*  see what ports are used to connect
		*  the STMicro Board with the HD44780 LCD driver
		*  Set appropriate pins as digital input/outputs
********************************/
void LCD_port_init(){
	//STEP 1: Enable GPIOD in RCC AHB1ENR register
	RCC -> AHB1ENR = 0x00000008;
	//STEP 2: Set MODER of GPIOD Pins 7, 6, 3, 2, 1 & 0 as outputs
	GPIOD -> MODER &=~(1<<15);
	GPIOD -> MODER |=(1<<14); //7
	GPIOD -> MODER &=~(1<<13);
	GPIOD -> MODER |=(1<<12); //6
	GPIOD -> MODER &=~(1<<7);
	GPIOD -> MODER |=(1<<6); //3
	GPIOD -> MODER &=~(1<<5);
	GPIOD -> MODER |=(1<<4); //2
	GPIOD -> MODER &=~(1<<3);
	GPIOD -> MODER |=(1<<2); //1
	GPIOD -> MODER &=~(1<<1);
	GPIOD -> MODER |=(1<<0); //0
	//STEP 3: Set OTYPER of GPIOD Pins 7, 6, 3, 2, 1 & 0 as push-pull
	GPIOD -> OTYPER &=~(1<<7);
	GPIOD -> OTYPER &=~(1<<6);
	GPIOD -> OTYPER &=~(1<<3);
	GPIOD -> OTYPER &=~(1<<2);
	GPIOD -> OTYPER &=~(1<<1);
	GPIOD -> OTYPER &=~(1<<0);
	//Done with LCD port Initialization}
	/*******************************
			*  LCD_init()
			*  Inputs: NONE
			*  Outputs: NONE
			*  LCD Initialization
			*  Read the manual carefully
			*  We are doing initialization by instruction
			*  Don't rush it.
	********************************/
}
	void LCD_init(){
		// STEP 1: Wait for 100ms for power-on-reset to take effect
		delay(100);
		// STEP 2: Set RS pin LOW to send instructions LCD pin 4
		GPIOD -> ODR &=~ (1<<RS);
		// Send instructions using following format:
		// Set EN=HIGH; Send 4-bit instruction; Set EN=low; delay 20ms;
		// STEP 3a-3d: Set 4-bit mode (takes a total of 4 steps)
		//DB4&DB5 == 1 pg. 46 of ref
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		
		delay(100);
		
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
			
		
		// STEP 4: Set 2 line display -- treats 16 char as 2 lines
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR |=(1<<DB7);//Db7
		GPIOD -> ODR |=(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable	
		delay(20);
		
		// STEP 5: Set DISPLAY to OFF

		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR |=(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		
		// STEP 6: CLEAR DISPLAY
		
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
	
		
		// STEP 7: SET ENTRY MODE - Auto increment; no scrolling
		
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR |=(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);

		delay(20);
		// STEP 8: Set Display to ON with Cursor and Blink.
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR |=(1<<DB5);//db5
		GPIOD -> ODR |=(1<<DB7);//db7
		GPIOD -> ODR |=(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
	
		GPIOD -> ODR |= (1<<RS);
	}
	/*******************************
		*  place_lcd_cursor()
		*  Inputs: unsigned character
		*  Outputs: NONE
		*  sets Cursor position to
		*  Line 1, character 1 (hex address 0x80)
		*  or Line 2, character 1 (hex addres 0xC0)
	*********************************/
void place_lcd_cursor(int lineno){
if (lineno == 1){
		GPIOD -> ODR &=~ (1<<RS);	
		//delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~0xF;
		GPIOD -> ODR |=0x8;
		//delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~0xF;
		//delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		GPIOD -> ODR |= (1<<RS);
		//delay(20);
}
if (lineno == 2){
		GPIOD -> ODR &=~ (1<<RS);	
		//delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR |=(1<<DB7);//db7
		GPIOD -> ODR |=(1<<DB6);//db6
		//delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		//delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable
		delay(20);
		GPIOD -> ODR |= (1<<RS);
}
}
	/********************************  
		*  LCD_write()
		*  Inputs: unsigned character data (8-bit)
		*  Outputs: NONE
		*  writes the character to LCD.
	*********************************/
void LCD_write(char data[]){
		int fh, sh, len, i;
		len = strlen(data);
	if(len>16){
		data = "String cant > 16";
	}
		GPIOD -> ODR |= (1<<RS);	
		for (i=0; i<len && i<8; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
}
		if (len>=8){
			place_lcd_cursor(2);
			for (i=8; i<len; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			}
		}
}

void reverse(char data[]){
			int fh, sh, len, i;
		len = strlen(data);
	if(len>16){
		data = "String cant > 16";
	}
		GPIOD -> ODR |= (1<<RS);	
		for (i=len-1; i>0; i--){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
}
		if (len>=8){
			place_lcd_cursor(2);
			for (i=len-9; i>=0; i--){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			}
		}
}

void oneatatime(char data[]){
		int fh, sh, len, i;
		len = strlen(data);
	if(len>16){
		data = "String cant > 16";
	}
		GPIOD -> ODR |= (1<<RS);	
		for (i=0; i<len && i<8; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(250);
}
		if (len>=8){
			place_lcd_cursor(2);
			for (i=8; i<len; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(250);
			}
		}
}

void vowel(char data[]){ 
int fh, sh, len, i;
		len = strlen(data);
	for(i=0;i<len;i++){
		if (data[i] == 'a' || data[i] =='e' || data[i] =='i' || data[i] =='o' || data[i] =='u'){
			data[i] = data[i] - 32;
		}
	}
	if(len>16){
		data = "String cant > 16";	}
		GPIOD -> ODR |= (1<<RS);	
		for (i=0; i<len && i<8; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
}
		if (len>=8){
			place_lcd_cursor(2);
			for (i=8; i<len; i++){
			GPIOD -> ODR &=~ 0xF;
			fh = (data[i]/0x10);
			sh = (data[i]%0x10);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR |= fh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			GPIOD -> ODR |=(1<<EN); //enable
			GPIOD -> ODR &=~ 0xF;
			GPIOD -> ODR |= sh;
			GPIOD -> ODR &=~(1<<EN); //enable
			delay(20);
			}
		}
}

void clear(void){
		GPIOD -> ODR &=~ (1<<RS);
	  GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR &=~(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |=(1<<EN); //enable
		GPIOD -> ODR |=(1<<DB4);//Db4
		GPIOD -> ODR &=~(1<<DB5);//db5
		GPIOD -> ODR &=~(1<<DB7);//Db7
		GPIOD -> ODR &=~(1<<DB6);//db6
		delay(10);
		GPIOD -> ODR &=~(1<<EN); //enable		
		delay(20);
		GPIOD -> ODR |= (1<<RS);
}

void number(int num){
	char data[100];
	
	sprintf(data, "%d", num);
	
	LCD_write(data);
}
	
	
	
	
	

