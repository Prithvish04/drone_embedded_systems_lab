
#ifndef PARSE_H_
#define PARSE_H_


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

typedef struct 
{
	uint8_t start;
	uint8_t mode;
	int16_t pitch;
	int16_t roll;
	int16_t yaw;
    int16_t lift;
	uint8_t stop;	
} drone_message;



long long to_dec(char hex[], int length); 
void print_message_block(int message[],int start_index, int length);
void process_key(uint8_t);

#endif /* PARSE_H_ */

