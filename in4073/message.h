#ifndef MESSAGE_H_
#define MESSAGE_H_


#include "in4073.h"
#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "adc.h"
#include "barometer.h"
#include "gpio.h"
#include "spi_flash.h"
#include "timers.h"
#include "twi.h"
#include "hal/uart.h"
#include "control.h"
#include "mpu6050/mpu6050.h"
#include "utils/quad_ble.h"
#include <stdlib.h>


struct message
{
	uint8_t start;
	
	uint32_t timestamp;
	
	uint16_t mot0;
	uint16_t mot1;
	uint16_t mot2;
	uint16_t mot3;
	
	int16_t phi;
	int16_t theta;
	int16_t psi;
	
	int16_t gyro0;
	int16_t gyro1;
	int16_t gyro2;
	
	uint16_t battery;
	uint32_t pressure;
	
	uint8_t stop;	
};

void toBinary(unsigned n, int len);
void print_message(struct message m);


#endif /* MESSAGE_H_ */
