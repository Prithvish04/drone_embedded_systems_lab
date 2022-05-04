
#ifndef _LINUX_COMM_H
#define _LINUX_COMM_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>



typedef struct {
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
	uint16_t lift;
	bool panic;
} js_data_type;

void print_raw_js_data();
js_data_type js_values_read();

#endif
