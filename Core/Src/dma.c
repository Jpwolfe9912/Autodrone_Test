/*
 * dma.c
 *
 *  Created on: Oct 22, 2021
 *      Author: jeremywolfe
 *
 *  SPI1_RX on DMA2_Stream2_CH3
 *  SPI1_TX on DMA2_Stream3_CH3
 */

#include "dma.h"

uint32_t *pval = SRAM1_MEMORY;		// set memory location as SRAM

void DMA2_Init(void){
	/*		Stream 2 (SPI1_RX) Config		*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

	DMA2_Stream2->CR &= ~DMA_SxCR_EN;		// disable stream 2
	while(DMA2_Stream2->CR & DMA_SxCR_EN){}	// wait until disabled

	DMA2_Stream2->PAR = (uint32_t)(&(SPI1->DR));// peripheral memory is SPI1
	DMA2_Stream2->M0AR = (uint32_t)(pval);	// memory address as val

	DMA2_Stream2->NDTR = 1;					// 1 data item

	DMA2_Stream2->CR |= (0x3 << 25U);		// channel 3 selection
	DMA2_Stream2->CR |= DMA_SxCR_PL;		// very high priority
	DMA2_Stream2->CR &= ~DMA_SxCR_DIR;		// peripheral to memory
	DMA2_Stream2->CR |= DMA_SxCR_CIRC |		// circular mode enabled
						DMA_SxCR_MINC;		// memory increment mode
	DMA2_Stream2->CR &= ~DMA_SxCR_MSIZE;	// 8 bit memory increment

	DMA2_Stream2->CR |= DMA_SxCR_EN;		// enable the stream
}









