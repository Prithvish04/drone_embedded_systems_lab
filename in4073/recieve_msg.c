#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "recieve_msg.h"


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