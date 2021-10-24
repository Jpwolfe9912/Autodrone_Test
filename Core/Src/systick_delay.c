/*
 * systick_delay.c
 *
 *  Created on: Jul 3, 2021
 *      Author: jeremywolfe
 */

#include "systick_delay.h"

int msTicks = 0;

void delay(int ms){
	SysTick_Config(216000000 / 1000);
	msTicks = 0;
	while(msTicks < ms){}
}

void SysTick_Handler(void){
	msTicks++;
}
