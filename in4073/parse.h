
#ifndef PARSE_H_
#define PARSE_H_


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#include "gpio.h"
#include "control.h"
#include "structs.h"

#define START_BYTE 0xAA
#define STOP_BYTE  0XA0


#define LIFT_UP_KEY    'a'
#define LIFT_DOWN_KEY  'z'
#define ROLL_UP_KEY    44
#define ROLL_DOWN_KEY  43
#define PITCH_DOWN_KEY 42
#define PITCH_UP_KEY   41
#define YAW_DOWN_KEY   'q'
#define YAW_UP_KEY     'w'
#define P_DOWN_KEY     'j'
#define P_UP_KEY       'u'
#define P1_DOWN_KEY    'k'
#define P1_UP_KEY      'i'
#define P2_DOWN_KEY    'l'
#define P2_UP_KEY      'o'


static const Events events_array[] = { Safe_Event,
									   Panic_Event,
									   Manual_Event,
									   Calibration_Event,
									   Yaw_Event,
									   Full_Event,
									   Raw_Event,
									   Height_Event,
									   Wireless_Event,
									   Null};


void process_key(uint8_t, DroneMessage*);
bool process_message(uint8_t, DroneMessage*);
void print_commands(DroneMessage*);
void log_measurement(uint32_t, uint16_t*, int16_t*, int16_t*, uint16_t, int32_t, int32_t, Measurement*);
void print_GUI(Modes, DroneMessage*, Measurement*);
void print_debug(char*);

#endif /* PARSE_H_ */

