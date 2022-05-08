#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "parse.h"

struct Message {
	uint8_t start;
	int16_t pitch;
	int16_t roll;
	int16_t yaw;
    int16_t lift;
    int8_t mode;
	uint8_t stop;	
};

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