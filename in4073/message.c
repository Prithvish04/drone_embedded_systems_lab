
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
	
	//printf("\n");
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

	printf("%02X", m.start); //2
	printf("%08lX", m.timestamp); //8
	printf("%04X", m.mot0); // 4
	printf("%04X", m.mot1); //4
	printf("%04X", m.mot2); //4
	printf("%04X", m.mot3); // 4
	printf("%04X", m.phi); //4
	printf("%04X", m.theta); //4
	printf("%04X", m.psi); //4 
	printf("%04X", m.gyro0); //4 
	printf("%04X", m.gyro1); //4
	printf("%04X", m.gyro2); //4
	printf("%04X", m.battery); //4
	printf("%08lX", m.pressure); //8
	printf("%02X", m.stop); //2


/*	
	//printf("%08" PRIx8, m.start);
	//printf("\n");
	printf("%d", m.start); //2
	printf("%d", m.timestamp); //8
	printf("%d", m.mot0); // 4
	printf("%d", m.mot1); //4
	printf("%d", m.mot2); //4
	printf("%d", m.mot3); // 4
	printf("%d", m.phi); //4
	printf("%d", m.theta); //4
	printf("%d", m.psi); //4 
	printf("%d", m.gyro0); //4 
	printf("%d", m.gyro1); //4
	printf("%d", m.gyro2); //4
	printf("%d", m.battery); //4
	printf("%d", m.pressure); //8
	printf("%d", m.stop); //2
	*/
}
