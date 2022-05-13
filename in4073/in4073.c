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
#include "message.h"
#include "parse.h"
#include "state_machine.h"

bool demo_done;
bool debug_drone= false;

/*------------------------------------------------------------------
 * main -- everything you need is here :)
 *------------------------------------------------------------------
 */
int main(void) {
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
	DroneMessage msg;
	bool isMsg = false;

	
	demo_done = false;
	wireless_mode = false;

	while (!demo_done) {

		if (rx_queue.count) {
			isMsg = process_message((dequeue(&rx_queue)), &msg);
		}
		/*
		if (ble_rx_queue.count) 
			process_key(dequeue(&ble_rx_queue));
		*/

		if (check_timer_flag()) {
			if (counter++%20 == 0) {
				nrf_gpio_pin_toggle(BLUE);
			}

			adc_request_sample();
			read_baro();
			
			m_log.start = 170; 
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
			m_log.stop = '\n';
			
			//printf("%10ld | \n", get_time_us());
			/*sprintf("%3d %3d %3d %3d | ",ae[0], ae[1], ae[2], ae[3]);
			printf("%6d %6d %6d | ", phi, theta, psi);
			printf("%6d %6d %6d | ", sp, sq, sr);
			printf("%4d | %4ld | %6ld \n", bat_volt, temperature, pressure);*/
			
			if(debug_drone){
				print_message_hex(m_log);
				printf("\n");

			}

			clear_timer_flag();
		}

		if (check_sensor_int_flag()) {
			get_sensor_data();
			run_filters_and_control();
		}

		if(isMsg) {
			printf("%d |", msg.mode);
			printf("%d |", msg.yaw);
			printf("%d |", msg.roll);
			printf("%d |", msg.pitch);
			printf("%d |\n", msg.lift);
		}
	}
	
	/*
	Events newEvent = read_event(struct *drone_message);
	if(StateMachine[nextState][newEvent]!= NULL)
    	nextState = (*StateMachine[nextState][newEvent])();
	*/

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
