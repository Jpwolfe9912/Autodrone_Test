/*
 * spi.h
 *
 *  Created on: Oct 17, 2021
 *      Author: jeremywolfe
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include "stm32f7xx.h"

void SPI1_GPIO_Init(void);
void SPI1_Init(void);
void SPI1_Read(uint8_t *pdata, int size);
void SPI1_Write(uint8_t *pdata, int size);

#endif /* INC_SPI_H_ */
