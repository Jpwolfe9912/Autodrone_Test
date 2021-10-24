/*
 * rcc.c
 *
 *  Created on: Oct 18, 2021
 *      Author: jeremywolfe
 *
 *  7 wait states
 *  voltage scale 1
 *  power overdrive enabled
 */

#include "rcc.h"

void RCC_216MHz_Init(void){
	RCC->CR &= ~RCC_CR_PLLON;			// PLL off
	while(RCC->CR & RCC_CR_PLLRDY){}	// wait til off
	RCC->CFGR &= ~RCC_CFGR_SW;			// HSI selected as system clock source
	while(!(RCC->CR & RCC_CFGR_SW)){}	// wait til HSI selected

	RCC->APB1ENR |= RCC_APB1ENR_PWREN;	// enable power
	PWR->CR1 |= PWR_CR1_VOS;			// voltage scale 1

	RCC->CR |= RCC_CR_HSION;			// turn on HSI
	while(!(RCC->CR & RCC_CR_HSIRDY)){}	// wait til HSI ready
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;// set PLL clk source as HSI

	/*		PLL Division Configuration	*/
	/*		16 / 8 = 2 * 216 = 432 / 2 = 216	*/
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLM;
	RCC->PLLCFGR |= 8U;					// PLLM /8 div
	RCC->PLLCFGR |= (216U << 6U);		// PLLN *216 multi
	RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP;	// PLLP /2 div
	RCC->CR |= RCC_CR_PLLON;			// turn on PLL

	/*		Overdrive enable	*/
	PWR->CR1 |= PWR_CR1_ODEN;
	while(!(PWR->CSR1 & PWR_CSR1_ODRDY)){}
	PWR->CR1 |= PWR_CR1_ODSWEN;
	while(!(PWR->CSR1 & PWR_CSR1_ODSWRDY)){}
	FLASH->ACR |= FLASH_ACR_LATENCY_7WS;
	while(!(FLASH->ACR & FLASH_ACR_LATENCY_7WS)){}

	/*		CPU, AHB, APB Bus Configuration	*/
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;		// AHB = 216MHz
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV4;		// APB1 = 216 / 4 = 54MHz
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV2;		// APB2 = 216 / 2 = 108MHz
	RCC->CFGR |= RCC_CFGR_SW_PLL;			// PLL selected as system clock
	while(!(RCC->CR & RCC_CR_PLLRDY)){}		// wait til PLL locked
	while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)){}// wait until selected
}
