#ifndef CONTROL_H_
#define CONTROL_H_

#include <inttypes.h>
#include <stdbool.h>

extern uint16_t motor[4];
extern int16_t ae[4];
extern bool wireless_mode;

void all_motors_hold_speed();
void all_motors_stop();
void all_motors_slow_down();




void run_filters_and_control();
void all_motors_grad_slow();

#endif /* CONTROL_H_ */
