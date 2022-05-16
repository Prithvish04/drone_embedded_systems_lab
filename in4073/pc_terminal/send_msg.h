
#ifndef MESSAGE_H_
#define MESSAGE_H_


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

#define MASK 0xFF

struct message {
	uint8_t start;
	uint8_t mode;
	uint8_t pitch_H;
	uint8_t pitch_L;
	uint8_t roll_H;
	uint8_t roll_L;
	uint8_t yaw_H;
	uint8_t yaw_L;
	uint8_t lift_H;
	uint8_t lift_L;
	uint8_t stop;	 
};

void print_message(struct message m);


#endif /* MESSAGE_H_ */
