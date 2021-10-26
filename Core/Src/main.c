#include "stm32f7xx.h"
#include "systick_delay.h"
#include "spi.h"
#include "rcc.h"
#include "mco.h"
#include "usart.h"
#include "ICM20984.h"

//extern uint16_t accel_data[3];
//extern uint16_t gyro_data[3];

int main(void){
	RCC_216MHz_Init();
	printUSART_Init();
	SPI1_Init();
	ICM_Init();

	uint8_t whoami;
	uint8_t foo = 0x80;
	while(1){
//		ICM_AccGyro_Read();
//		while(!ICM_WHOAMI_Ready()){}
		ICM_Read(WHO_AM_I_ICM20948, &whoami, 1);
//		printf("ICM Name is: %X\n", whoami);
//		ICM_WriteRead(WHO_AM_I_ICM20948, &whoami);
//		ICM_WriteRead(WHO_AM_I_ICM20948, &whoami);
//		printf("Who am I is: %X", whoami);
//		ICM_WriteTest(whoami);

//		ICM_HAL_WriteRead(&foo, &whoami, 1);
		printf("%X\n", whoami);
	}
}
