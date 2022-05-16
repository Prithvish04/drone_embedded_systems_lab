
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
#define STOP_BYTE  0XA

#define LEFT_KEY  0x44
#define RIGHT_KEY 0x43
#define DOWN_KEY  0x42
#define UP_KEY    0x41


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
void log_measurement(uint32_t, int16_t*, int16_t*, int16_t*, uint16_t, int32_t, int32_t, Measurement*);
void print_measurement(Measurement*);
void print_GUI(Modes, DroneMessage*, Measurement*);

#endif /* PARSE_H_ */

