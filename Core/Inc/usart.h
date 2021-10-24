/*
 * usart.h
 *
 *  Created on: Oct 19, 2021
 *      Author: jeremywolfe
 */

#ifndef INC_USART_H_
#define INC_USART_H_

#include "stm32f7xx.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"

void GPIO_USART_Init(void);
void printUSART_Init(void);
int USART3_Write(int ch);

#endif /* INC_USART_H_ */
