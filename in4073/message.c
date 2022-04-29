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
#include "message.h"


void print_message(struct message m)
{
	toBinary(m.start, 8);
	toBinary(m.timestamp, 32);
	toBinary(m.mot0, 16);
	toBinary(m.mot1, 16);
	toBinary(m.mot2, 16);
	toBinary(m.mot3, 16);
	toBinary(m.phi, 16);
	toBinary(m.theta, 16);
	toBinary(m.psi, 16);
	toBinary(m.gyro0, 16);
	toBinary(m.gyro1, 16);
	toBinary(m.gyro2, 16);
	toBinary(m.battery, 16);
	toBinary(m.pressure, 32);
	toBinary(m.stop, 8);
	
	printf("\n");
}

void toBinary(unsigned n, int len)
{
	for (unsigned i = (1 << (len - 1)); i > 0; i = i/2)
	{
		(n & i) ? printf("1"):printf("0"); 
	}
}
