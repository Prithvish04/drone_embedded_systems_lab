/*------------------------------------------------------------------
 *  control.c -- here you can implement your control algorithm
 *		 and any motor clipping or whatever else
 *		 remember! motor input =  0-1000 : 125-250 us (OneShot125)
 *
 *  I. Protonotarios
 *  Embedded Software Lab
 *
 *  July 2016
 *------------------------------------------------------------------
 */

#include "control.h"
#include <inttypes.h>
#include <stdbool.h>
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "in4073.h"
#include "utils/quad_ble.h"
#include "utils/queue.h"
#include "mpu6050/mpu6050.h"
#include "uart.h"
#include "gpio.h"


uint16_t motor[4];
int16_t ae[4];
bool wireless_mode;

// gotta use this one for control algorithms
// seems like motor values are truncated between 0 and 1000 within the timers.c code
void update_motors(void){
	motor[0] = ae[0];
	motor[1] = ae[1];
	motor[2] = ae[2];
	motor[3] = ae[3];                                                                                         
}

void set_motors(int16_t m0, int16_t m1, int16_t m2, int16_t m3) {
	ae[0] = (m0 < MAX_RPM) ? ((m0 < MIN_RPM) ? ((m0 <= 0) ? 0 : MIN_RPM): m0) : MAX_RPM;
	ae[1] = (m1 < MAX_RPM) ? ((m1 < MIN_RPM) ? ((m1 <= 0) ? 0 : MIN_RPM): m1) : MAX_RPM;
	ae[2] = (m2 < MAX_RPM) ? ((m2 < MIN_RPM) ? ((m2 <= 0) ? 0 : MIN_RPM): m2) : MAX_RPM;
	ae[3] = (m3 < MAX_RPM) ? ((m3 < MIN_RPM) ? ((m3 <= 0) ? 0 : MIN_RPM): m3) : MAX_RPM;
}

int32_t map_limits(int32_t upper, int32_t lower, int32_t low_input, int32_t high_input, int32_t value) {
	return (((value - low_input) * (upper-lower)) / (high_input-low_input)) + lower;
}

bool check_neutral(DroneMessage* msg) {
	return (msg->yaw == 0) & (msg->pitch == 0) & (msg->roll == 0) & (msg->lift == 32766);
}

void add_imu_offset(Measurement* msg) {
	msg->phi -= msg->phi_offset;  msg->psi -= msg->psi_offset;  msg->theta -= msg->theta_offset;
	msg->sp -= msg->sp_offset;    msg->sq -= msg->sq_offset;    msg->sr -= msg->sr_offset;
	msg->sax -= msg->sax_offset;  msg->say -= msg->say_offset;  msg->saz -= msg->saz_offset; 
}

uint32_t sqrt_32(uint32_t a_nInput){
    uint32_t op  = a_nInput;
    uint32_t res = 0;
    uint32_t one = 1uL << 30; // The second-to-top bit is set: use 1u << 14 for uint16_t type; use 1uL<<30 for uint32_t type
	// "one" starts at the highest power of four <= than the argument.
    while (one > op)
        one >>= 2;

    while (one != 0) {
        if (op >= res + one) {
            op = op - (res + one);
            res = res +  2 * one;
        }
        res >>= 1;
        one >>= 2;
    }
    /* Do arithmetic rounding to nearest integer */
    if (op > res)
        res++;
    return res;
}