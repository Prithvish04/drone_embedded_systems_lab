#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include "nrf_gpio.h"
#include "parse.h"
#include "gpio.h"
#include "control.h"
#include <stdio.h>

bool demo_done;
bool recieving = false;


long long to_dec(char hex[], int length) {
	long long decimal = 0, base = 1;
	for(int i = length--; i >= 0; i--) {
		if(hex[i] >= '0' && hex[i] <= '9'){
		    decimal += (hex[i] - 48) * base;
		    base *= 16;
		}
		else if(hex[i] >= 'A' && hex[i] <= 'F'){
		    decimal += (hex[i] - 55) * base;
		    base *= 16;
		}
		else if(hex[i] >= 'a' && hex[i] <= 'f'){
		    decimal += (hex[i] - 87) * base;
		    base *= 16;
		}
	}
	return decimal;
}

void print_message_block(int message[],int start_index, int length) 
{
    char buffer[length+1]; 
    for (int i =start_index; i<start_index+length; i++)
    {
        buffer[i-start_index] = message[i];
    } 
    buffer[length] = '\n';
    printf("%lld |",to_dec(buffer,length-1));
}


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
		//panic mode 
		break;
	default:
		nrf_gpio_pin_toggle(RED);
	}
}


bool process_message(uint8_t c, DroneMessage* msg) {	
	static bool isValid = false;
	static uint8_t curIdx = MSG_SIZE;
	if((c == START_BYTE) && !isValid){
		isValid = true;
		curIdx = MSG_SIZE;
	}
	else if (isValid) {
		switch (curIdx) {
		case MSG_SIZE:
			msg->mode = c - 48;
			break;
		case MSG_SIZE-1:
			msg->pitch = (c - 48) << 8;
			break;
		case MSG_SIZE-2:
			msg->pitch += (c - 48);
			break;
		case MSG_SIZE-3:
			msg->roll = (c - 48) << 8;
			break;
		case MSG_SIZE-4:
			msg->roll += (c - 48);
			break;
		case MSG_SIZE-5:
			msg->yaw = (c - 48) << 8;
			break;
		case MSG_SIZE-6:
			msg->yaw += (c - 48);
			break;
		case MSG_SIZE-7:
			msg->lift = (c - 48) << 8;
			break;
		case MSG_SIZE-8:
			msg->lift += (c - 48);
			isValid = false;
			return true;
		}
		curIdx--;
	}
	return false;
}