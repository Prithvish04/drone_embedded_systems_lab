
#ifndef _LINUX_COMM_H
#define _LINUX_COMM_H

#include <stdio.h>

struct js_2_drone
{
	uint8_t start_bit;
	uint16_t pitch;
	uint16_t roll;
	uint16_t yaw;
	uint16_t lift;
	uint8_t mode;
	uint8_t end_bit;
	
}