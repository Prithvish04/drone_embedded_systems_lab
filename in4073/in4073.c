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
#include "parse.h"
#include "state_machine.h"
#include "profiler.h"

#define PANIK_CNT 10


bool demo_done;

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

	// Profilers initiation
	time_struct msgRecieve;

	uint32_t counter = 0;
	Modes curMode = Safe_Mode;
	Measurement m_log;
	DroneMessage command_buf[2];
	DroneMessage* msg = command_buf;
	int16_t euler[] = {0, 0, 0};
	int16_t imu[] = {0, 0, 0, 0, 0, 0};
	bool isMsg;
	uint8_t write_idx = 0;
	uint8_t panik_cnt = 0;
	bool rcvd = false;

	demo_done = false;
	wireless_mode = false;
	sensor_mode = true;
	isMsg = false;

	for (uint8_t i = 0; i < 2; ++i) {
		command_buf[i].event = Null;
		command_buf[i].lift_offset = 0;
		command_buf[i].roll_offset = 0;
		command_buf[i].pitch_offset = 0;
		command_buf[i].yaw_offset = 0;
		command_buf[i].P = 0;
		command_buf[i].P1 = 0;
		command_buf[i].P2 = 0;
	}

	m_log.phi_offset = 0;
	m_log.psi_offset = 0;
	m_log.theta_offset = 0;
	m_log.sp_offset = 0;
	m_log.sq_offset = 0;
	m_log.sr_offset = 0;
	m_log.sax_offset = 0;
	m_log.say_offset = 0;
	m_log.saz_offset = 0;

	while (!demo_done) {
		// get the first available valid message sequence
		if (rx_queue.count) {
			getstarttime(msgRecieve);
			isMsg = process_message((dequeue(&rx_queue)), command_buf + write_idx);
			if (isMsg) {
				msg = command_buf + write_idx;
				write_idx = (write_idx + 1) % 2;
			}
			rcvd = true;
			getstoptime(msgRecieve);
		}

		/*
		if (ble_rx_queue.count) 
			process_key(dequeue(&ble_rx_queue));
		*/

		// current timer is at 50 ms, we can tune this later depending on code execution timing
		// current if segment timing is around 2 ms (including gui printing)

		// TODO edit layout and displays, fix drone orientation

		if (check_timer_flag()) {
			if (counter++%20 == 0) 
				nrf_gpio_pin_toggle(BLUE);

			if (!rcvd) {
				if (panik_cnt++ == PANIK_CNT)
					msg->event = Panic_Event;
			} else
				panik_cnt = 0;

			adc_request_sample();
			read_baro();
	
			//print_commands(msg);
	       print_GUI(curMode, msg, &m_log); 
		   //printf("debug msg Recieve: %ld",timediff(msgRecieve) );

			clear_timer_flag();
			rcvd = false;
		}

		if (check_sensor_int_flag()) {
			get_sensor_data();
			euler[0] = phi;
			euler[1] = theta;
			euler[2] = psi;
			imu[0] = sp;
			imu[1] = sq;
			imu[2] = sr;
			imu[3] = sax;
			imu[4] = say;
			imu[5] = saz;
			log_measurement(get_time_us(), motor, euler, imu, bat_volt, temperature, pressure, &m_log);
			if (curMode != Calibration_Mode)
				add_imu_offset(&m_log);
		}

		if(StateMachine[curMode][msg->event] == NULL)
			msg->event = Null;
    	curMode = (*StateMachine[curMode][msg->event])(curMode, msg, &m_log);
	}	

	printf("\n\t Goodbye \n\n");
	nrf_delay_ms(100);

	NVIC_SystemReset();
}
