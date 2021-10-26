/*
 * spi.c
 *
 *  Created on: Oct 17, 2021
 *      Author: jeremywolfe
 *
 *  MPU-6000 on SPI1
 *  SPI1 on APB2
 *  MOSI on PA7
 *  MISO on PA6
 *  SCK  on PA5
 *  CS   on PA4
 *
 *  All on AF5
 */

#include "spi.h"


void _SPI1_GPIO_Init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

	GPIOA->MODER &= ~(GPIO_MODER_MODER4 |	// clear mode registers
			GPIO_MODER_MODER5 |
			GPIO_MODER_MODER6 |
			GPIO_MODER_MODER7);

	GPIOA->MODER |= GPIO_MODER_MODER4_1;	// set PA4 as output
	GPIOA->MODER |= GPIO_MODER_MODER5_1;	// set PA5 as AF
	GPIOA->MODER |= GPIO_MODER_MODER6_1;	// set PA6 as AF
	GPIOA->MODER |= GPIO_MODER_MODER7_1;	// set PA7 as AF

	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4 |
			GPIO_OSPEEDR_OSPEEDR5 |
			GPIO_OSPEEDR_OSPEEDR6 |
			GPIO_OSPEEDR_OSPEEDR7;

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFRL4 |	// clear AF registers
			GPIO_AFRL_AFRL5 |
			GPIO_AFRL_AFRL6 |
			GPIO_AFRL_AFRL7);
	GPIOA->AFR[0] |= (0x5 << 16U);			// set AF as SPI1_SCK
	GPIOA->AFR[0] |= (0x5 << 20U);			// set AF as SPI1_SCK
	GPIOA->AFR[0] |= (0x5 << 24U);			// set AF as SPI1_MISO
	GPIOA->AFR[0] |= (0x5 << 28U);			// set AF as SPI1_MOSI
}

void SPI1_Init(void) {
	_SPI1_GPIO_Init();

	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	/*		CR1 Initialization	*/
	SPI1->CR1 |= SPI_CR1_BR_0 |	// set baudrate (108MHz / 64 = 1.6875 < 7)
//			SPI_CR1_BR_1;// |
			SPI_CR1_BR_2;
	SPI1->CR1 |= SPI_CR1_CPOL |		// SCK idle state high
			SPI_CR1_CPHA;		// capture on second SCK edge (rising)
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;	// 2 unidirectional lines
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;	// data transmitted MSB first
	SPI1->CR1 &= ~SPI_CR1_SSM;		// CS done by hardware
	SPI1->CR1 |= SPI_CR1_MSTR;		// configure as master

	/*		CR2 Initialization	*/
	SPI1->CR2 |= ((8U - 1U) << 8U);	// 8 bit data length
	SPI1->CR2 &= ~SPI_CR2_FRF;		// motorola format
	SPI1->CR2 |= SPI_CR2_SSOE;		// NSS output enabled
//	SPI1->CR2 |= SPI_CR2_FRXTH;
}

void SPI1_Read(uint8_t *pdata, int size){
//	while(size){
		while(!(SPI1->SR & ~SPI_SR_FTLVL)){}
		while(SPI1->SR & SPI_SR_BSY){}		// wait til not busy
		SPI1->DR = 0;						// send dummy byte
		while(!(SPI1->SR & SPI_SR_RXNE)){}	// wait until data received
//		(*(uint8_t*)pdata++) = *(volatile uint8_t*)SPI1->DR;	// read int

		*pdata = (SPI1->DR >> 8U);	// read int
		while(!(SPI1->SR & ~SPI_SR_FRLVL)){}
		SPI1->CR1 &= ~SPI_CR1_SPE;
//		*pdata++ = SPI1->DR;
//		size--;
//	}
}

void SPI1_Write(uint8_t *pdata, int size){
	while(size){
		while (!(SPI1->SR & SPI_SR_TXE)){}	// wait until data transmitted
//		*((volatile uint8_t*)SPI1->DR) = *pdata;// send register to write to
		SPI1->DR = *pdata;
		pdata += sizeof(uint8_t);
		size--;
	}
	while(!(SPI1->SR & SPI_SR_TXE)){}		// wait until TX buffer empty
	while(SPI1->SR & SPI_SR_BSY){}			// wait until SPI not in communication
	volatile uint8_t temp = SPI1->DR;				// clear overrun flag by reading DR and SR
	temp = SPI1->SR;
}

void SPI1_WriteTest(uint8_t data){
//	while (!(SPI1->SR & SPI_SR_TXE)){}		// wait until data transmitted
	SPI1->DR = data;
	while(!(SPI1->SR & SPI_SR_TXE)){}		// wait until TX buffer empty
	while(SPI1->SR & SPI_SR_BSY){}			// wait until SPI not in communication
//	volatile uint8_t temp = SPI1->DR;				// clear overrun flag by reading DR and SR
//	temp = SPI1->SR;
}


void SPI1_WriteRead(uint8_t TxData, uint8_t *pRxData){
	SPI1->DR = TxData;
	while(!(SPI1->SR & SPI_SR_TXE)){}		// wait until TX buffer empty
	while(SPI1->SR & SPI_SR_BSY){}			// wait until SPI not in communication
	volatile uint8_t temp = SPI1->DR;				// clear overrun flag by reading DR and SR
	temp = SPI1->SR;

	while(SPI1->SR & SPI_SR_BSY){}		// wait til not busy
	SPI1->DR = 0;						// send dummy byte
	while(!(SPI1->SR & SPI_SR_RXNE)){}
	while(!(SPI1->SR & SPI_SR_FRLVL)){}
	*pRxData = SPI1->DR;
}



void SPI1_HAL_WriteRead(uint8_t *pTxData, uint8_t *pRxData, uint16_t size){
	uint32_t txallowed = 1U;
	uint16_t initTxCount = size;
	uint16_t initRxCount = size;
	uint16_t txCount = size;
	uint16_t rxCount = size;

	uint8_t *pRxBuff = (uint8_t*)pRxData;
	uint8_t *pTxBuff = (uint8_t*)pTxData;

	if(initRxCount > 1U){
		SPI1->CR2 &= ~SPI_CR2_FRXTH;			// 16 bit FIFO reception data length
	}
	else{
		SPI1->CR2 |= SPI_CR2_FRXTH;
	}
	if((SPI1->CR1 & SPI_CR1_SPE) != SPI_CR1_SPE){
		SPI1->CR1 |= SPI_CR1_SPE;
	}

	/*		TX and RX Data in 8 Bit Mode		*/
	if(initTxCount == 0x01U){
		if(txCount > 1U){
//			SPI1->DR = *((uint16_t*)pTxBuff);
			SPI1->DR = *pTxBuff;
			pTxBuff += sizeof(uint16_t);
			txCount -= 2U;
		}
		else{
			*(__IO uint8_t*)SPI1->DR = *pTxBuff;
			pTxBuff++;
			txCount--;
		}
	}
	while((txCount > 0U) || (rxCount > 0U)){
		if(!(SPI1->SR & SPI_SR_TXE) && (txCount > 0U) && (txallowed)){
			if(txCount > 1U){
//				SPI1->DR = *((uint16_t*)pTxBuff);
				SPI1->DR = *pTxBuff;
				pTxBuff += sizeof(uint16_t);
				txCount -= 2U;
			}
			else{
				*(__IO uint8_t*)SPI1->DR = *pTxBuff;
				pTxBuff++;
				txCount--;
			}
			txallowed = 0U;
		}
		if(!(SPI1->SR & SPI_SR_RXNE) && (rxCount > 0U)){
			if(rxCount > 1){
//				*(uint16_t*)pRxBuff = (uint16_t)SPI1->DR;
				*pRxBuff = SPI1->DR;
				pRxBuff += sizeof(uint16_t);
				pRxBuff -= 2U;
				if(rxCount <= 1U){
					SPI1->CR2 |= SPI_CR2_FRXTH;
				}
			}
			else{
//				(*(uint8_t*)pRxBuff) = *(__IO uint8_t*)SPI1->DR;
				*pRxBuff = SPI1->DR;
				pRxBuff++;
				rxCount--;
			}
			txallowed = 1U;
		}
	}

}










