/*
 * usart.c
 *
 *  Created on: Oct 19, 2021
 *      Author: jeremywolfe
 *
 *  PA9  on USART1_TX
 *  PA10 on USART1_RX
 *
 *  USART1 on AF7
 */


#include "usart.h"

void GPIO_USART_Init(void){
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	GPIOD->MODER &= ~(GPIO_MODER_MODER8 | GPIO_MODER_MODER9);
	GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
	GPIOD->AFR[1] |= (0x7 << 0U);
	GPIOD->AFR[1] |= (0x7 << 4U);

}

void printUSART_Init(void){
	GPIO_USART_Init();
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

	USART3->BRR = 0x1D5;		// 115200 baudrate
	USART3->CR1 |= USART_CR1_UE | USART_CR1_TE;
}

int USART3_Write(int ch){
	while (!(USART3->ISR & USART_ISR_TXE)){}	// waits for TX buffer to become empty
	USART3->TDR = ch;								// transfers the value of the data register into ch
	return 0;
}

/*	This is required to use printf											*/
/*	This basically tells the compiler what to do when it encounters printf	*/
/*	I honestly can't fully explain what is going on but it works			*/
#ifdef __GNUC__
	#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//	#define GETCHAR_PROTOTYPE int __io_getchar (void)
#else
	#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//	#define GETCHAR_PROTOTYPE int fgetc(FILE * f)
#endif

PUTCHAR_PROTOTYPE{
	USART3_Write(ch);
	return ch;
}







