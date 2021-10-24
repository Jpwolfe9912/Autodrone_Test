#include "stm32f7xx.h"
#include "systick_delay.h"
#include "spi.h"
#include "rcc.h"
#include "mco.h"
#include "usart.h"
#include "ICM20984.h"

extern uint16_t accel_data[3];
extern uint16_t gyro_data[3];

int main(void){
	RCC_216MHz_Init();
	printUSART_Init();
	SPI1_Init();
	ICM_Init();

	while(1){
		ICM_AccGyro_Read();
	}
}
