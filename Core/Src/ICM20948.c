/*
 * ICM20948.c
 *
 *  Created on: Oct 20, 2021
 *      Author: jeremywolfe
 */

#include "ICM20984.h"
#include "spi.h"

uint16_t accel_data[3];
uint16_t gyro_data[3];
uint16_t temp;


void ICM_Read(uint8_t reg, uint8_t *pdata, int size){
	reg |= 0x80;				// 8th bit high for a read
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1_Write(&reg, 1);
	SPI1_Read(pdata, size);
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void ICM_Write(uint8_t reg, uint8_t data, int size){
	reg &= ~0x80;			// 8th bit low for a write
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1_Write(&reg, 1);
	SPI1_Write(&data, size);
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void ICM_WriteTest(uint8_t data){
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1_WriteTest(data);
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void ICM_WriteRead(uint8_t TxData, uint8_t *pRxData){
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1_WriteRead(TxData, pRxData);
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void ICM_HAL_WriteRead(uint8_t *TxData, uint8_t *RxData, uint16_t size){
	*TxData |= 0x80;
	SPI1->CR1 |= SPI_CR1_SPE;
	SPI1_HAL_WriteRead(TxData, RxData, size);
	SPI1->CR1 &= ~SPI_CR1_SPE;
}

void ICM_AccGyro_Read(void){
	uint8_t raw_data[12];

	ICM_Read(ACCEL_XOUT_H, raw_data, 12);
	accel_data[0] = (raw_data[0] << 8) | raw_data[1];
	accel_data[1] = (raw_data[2] << 8) | raw_data[3];
	accel_data[2] = (raw_data[4] << 8) | raw_data[5];

	gyro_data[0] = (raw_data[6] << 8) | raw_data[7];
	gyro_data[1] = (raw_data[8] << 8) | raw_data[9];
	gyro_data[2] = (raw_data[10] << 8) | raw_data[11];
}

void ICM_Init(void){
//	while(!ICM_WHOAMI_Ready()){}				// wait until device ready

	ICM_Reset();					// reset device
	ICM_Wakeup();					// wake out of sleep mode

	ICM_ClockSource(1);
	ICM_ODREnable();

	ICM_SPI();

}

int ICM_WHOAMI_Ready(void){
	uint8_t whoami;
	ICM_Read(WHO_AM_I_ICM20948, &whoami, 1);
	if(whoami == DEVICE_ID){
		return 1;
	}
	else{
		return 0;
	}
}

void ICM_Reset(void){
	ICM_Write(PWR_MGMT_1, (0x80 | 0x41), 1);
	delay(100);
}

void ICM_Wakeup(void){
	uint8_t new_val;
	ICM_Read(PWR_MGMT_1, &new_val, 1);
	new_val &= 0xBF;
	ICM_Write(PWR_MGMT_1, new_val, 1);
	delay(100);
}

void ICM_ClockSource(uint8_t source){
	uint8_t new_val;
	ICM_Read(PWR_MGMT_1, &new_val, 1);
	new_val |= source;
	ICM_Write(PWR_MGMT_1, new_val, 1);
}

void ICM_ODREnable(void){
	ICM_Write(ODR_ALIGN_EN, 0x01, 1);
}

void ICM_SPI(void){
	uint8_t new_val;
	ICM_Read(USER_CTRL, &new_val, 1);
	new_val |= 0x10;
	ICM_Write(USER_CTRL, new_val, 1);
}









