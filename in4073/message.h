#ifndef MESSAGE_H_
#define MESSAGE_H_


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>


struct message
{
	uint8_t start;
	
	uint32_t timestamp;
	
	uint16_t mot0;
	uint16_t mot1;
	uint16_t mot2;
	uint16_t mot3;
	
	int16_t phi;
	int16_t theta;
	int16_t psi;
	
	int16_t gyro0;
	int16_t gyro1;
	int16_t gyro2;
	
	uint16_t battery;
	uint32_t pressure;
	
	uint8_t stop;
	
	/*int start;
	
	int timestamp;
	
	int mot0;
	int mot1;
	int mot2;
	int mot3;
	
	int phi;
	int theta;
	int psi;
	
	int gyro0;
	int gyro1;
	int gyro2;
	
	int battery;
	int pressure;
	
	int stop;*/		
};

void toBinary(unsigned n, int len);
void print_message(struct message m);
void print_message_hex(struct message m);


#endif /* MESSAGE_H_ */
