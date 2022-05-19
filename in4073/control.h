#ifndef CONTROL_H_
#define CONTROL_H_

#include <inttypes.h>
#include <stdbool.h>
#include "structs.h"

#define MAX_RPM 450
#define MIN_RPM 150


extern uint16_t motor[4];
extern int16_t ae[4];
extern bool wireless_mode;


void update_motors();
void set_motors(int16_t, int16_t, int16_t, int16_t);
int32_t map_limits(int32_t, int32_t, int32_t, int32_t, int32_t); 
bool check_neutral(DroneMessage*);
void add_euler_offset(Measurement*);

#endif /* CONTROL_H_ */
