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

bool demo_done;


long long to_dec(char hex[], int length)
{
	long long decimal = 0, base = 1;
	for(int i = length--; i >= 0; i--)
	{
		if(hex[i] >= '0' && hex[i] <= '9')
		{
		    decimal += (hex[i] - 48) * base;
		    base *= 16;
		}
		else if(hex[i] >= 'A' && hex[i] <= 'F')
		{
		    decimal += (hex[i] - 55) * base;
		    base *= 16;
		}
		else if(hex[i] >= 'a' && hex[i] <= 'f')
		{
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

/*------------------------------------------------------------------
 * process_key -- process command keys
 *------------------------------------------------------------------
 */


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


bool process_message(uint8_t c){
	if(c == 0xAA){
		printf("first byte");
	}
	return false;
}