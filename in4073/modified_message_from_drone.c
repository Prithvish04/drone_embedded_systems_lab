/*------------------------------------------------------------------
 *  in4073.c -- test QR engines and sensors
 *
 *  reads ae[0-3] uart rx queue
 *  (q,w,e,r increment, a,s,d,f decrement)
 *
 *  prints timestamp, ae[0-3], sensors to uart tx queue
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  June 2016
 *------------------------------------------------------------------
 */
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

bool demo_done;


/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */
void process_key(uint8_t c)
{
	switch (c) {
	case 'q':
		ae[0] += 10;
		break;
	case 'a':
		ae[0] -= 10;
		if (ae[0] < 0) ae[0] = 0;
		break;
	case 'w':
		ae[1] += 10;
		break;
	case 's':
		ae[1] -= 10;
		if (ae[1] < 0) ae[1] = 0;
		break;
	case 'e':
		ae[2] += 10;
		break;
	case 'd':
		ae[2] -= 10;
		if (ae[2] < 0) ae[2] = 0;
		break;
	case 'r':
		ae[3] += 10;
		break;
	case 'f':
		ae[3] -= 10;
		if (ae[3] < 0) ae[3] = 0;
		break;
	case 27:
		demo_done = true;
		break;
	default:
		nrf_gpio_pin_toggle(RED);
	}
}

struct message
{
	//uint8_t start;
	
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

void toBinary(unsigned n, int len)
{
	for (unsigned i = (1 << (len - 1)); i > 0; i = i/2)
	{
		(n & i) ? printf("1"):printf("0"); 
	}
}

void print_message(struct message m)
{
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
	
	printf("\n");
}


/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void)
{
	uart_init();
	gpio_init();
	timers_init();
	adc_init();
	twi_init();
	imu_init(true, 100);
	baro_init();
	spi_flash_init();
	quad_ble_init();

	uint32_t counter = 0;
	struct message m_log;
	
	demo_done = false;
	wireless_mode = false;

	while (!demo_done) {
		if (rx_queue.count) {
			process_key(dequeue(&rx_queue));
		}
		if (ble_rx_queue.count) {
			process_key(dequeue(&ble_rx_queue));
		}

		if (check_timer_flag()) {
			if (counter++%20 == 0) {
				nrf_gpio_pin_toggle(BLUE);
			}

			adc_request_sample();
			read_baro();
			
			//m_log.start = 
			m_log.timestamp = get_time_us();
			m_log.mot0 = ae[0];
			m_log.mot1 = ae[1];
			m_log.mot2 = ae[2];
			m_log.mot3 = ae[3];
			m_log.phi = phi;
			m_log.theta = theta;
			m_log.psi = psi;
			m_log.gyro0 = sp;
			m_log.gyro1 = sq;
			m_log.gyro2 = sr;
			m_log.battery = bat_volt;
			m_log.pressure = pressure;
			m_log.stop = '\n'
			

			/*printf("%10ld | ", get_time_us());
			printf("%3d %3d %3d %3d | ",ae[0], ae[1], ae[2], ae[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);*/
			
			print_message(m_log);

			clear_timer_flag();
		}

		if (check_sensor_int_flag()) {
			get_sensor_data();
			run_filters_and_control();
		}
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
