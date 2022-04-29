
#include "message.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


void print_message(struct message m)
{
	toBinary(m.start, 8);
	toBinary(m.timestamp, 32);
	toBinary(m.mot0, 16);
	toBinary(m.mot1, 16);
	toBinary(m.mot2, 16);
	toBinary(m.mot3, 16);
	toBinary(m.phi, 16);
	toBinary(m.theta, 16);
	toBinary(m.psi, 16);
	toBinary(m.gyro0, 16);
	toBinary(m.gyro1, 16);
	toBinary(m.gyro2, 16);
	toBinary(m.battery, 16);
	toBinary(m.pressure, 32);
	toBinary(m.stop, 8);
	
	printf("\n");
}

void toBinary(unsigned n, int len)
{
	for (unsigned i = (1 << (len - 1)); i > 0; i = i/2)
	{
		(n & i) ? printf("1"):printf("0"); 
	}
}

void print_message_hex(struct message m)
{
	printf("%02X", m.start);
	printf("%08lX", m.timestamp);
	printf("%04X", m.mot0);
	printf("%04X", m.mot1);
	printf("%04X", m.mot2);
	printf("%04X", m.mot3);
	printf("%04X", m.phi);
	printf("%04X", m.theta);
	printf("%04X", m.psi);
	printf("%04X", m.gyro0);
	printf("%04X", m.gyro1);
	printf("%04X", m.gyro2);
	printf("%04X", m.battery);
	printf("%08lX", m.pressure);
	printf("%02X", m.stop);
	//printf("%08" PRIx8, m.start);
	printf("\n");
}
