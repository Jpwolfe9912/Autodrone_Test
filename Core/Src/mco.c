/*
 * mco.c
 *
 *  Created on: Sep 2, 2021
 *      Author: jeremywolfe
 *
 *      MCO2 on PC9
 */


#include "mco.h"

void MCO2_Init(void){

	/*		MCO2 Init	*/
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;		// enable clock for port C
	RCC->CFGR &= ~RCC_CFGR_MCO2;				// system clock clock selected
	RCC->CFGR |= (0x6 << 27U);					// prescalar of 4 so o-scope can read

	GPIOC->MODER &= ~GPIO_MODER_MODER9;			// clear bit
	GPIOC->MODER |= GPIO_MODER_MODER9_1;			// set as alternate function
	GPIOC->AFR[1] &= ~GPIO_AFRH_AFRH1;			// set AF as MCO1

}
