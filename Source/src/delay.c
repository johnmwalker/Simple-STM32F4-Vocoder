/*
Vocoder Project for ENGR 331
John Walker and Andrew Whately
May 2018

This file manages delays. Currently, the systick interrupt is set
to go off once every microsecond. This gives the LCD code the 
ability to delay for extremely brief periods of time.
*/

#include "delay.h"

static __IO uint32_t sysTickCounter;
static uint32_t usec_counter=0;
static int enableCounting=0;

// This function is never actually called in our code.
void SysTick_Init(void) {
	/****************************************
	 *SystemFrequency/1000      1ms         *
	 *SystemFrequency/100000    10us        *
	 *SystemFrequency/1000000   1us         *
	 *****************************************/
	while (SysTick_Config(SystemCoreClock / 1000000) != 0) {
	} // One SysTick interrupt now equals 1us

}

/**
 * This method needs to be called in the SysTick_Handler
 */
void TimeTick_Decrement(void) {
	if (sysTickCounter != 0x00) {
		sysTickCounter--;
	}
	if (enableCounting)
	{
		usec_counter++;
	}
}

void delay_nus(u32 n) {
	sysTickCounter = n;
	while (sysTickCounter != 0) {
	}
}

void delay_1ms(void) {
	sysTickCounter = 1000;
	while (sysTickCounter != 0) {
	}
}

void delay_nms(u32 n) {
	while (n--) {
		delay_1ms();
	}
}

void toggleCounter(void)
{
	if(enableCounting)
	{
		enableCounting=0;
		usec_counter = 0;
	} else if(!enableCounting)
	{
		enableCounting=1;
		usec_counter = 0;
	}
}

uint32_t counterValue(void)
{
	return usec_counter;
}
