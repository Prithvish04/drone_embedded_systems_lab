
#ifndef MESSAGE_H_
#define MESSAGE_H_


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

struct message {
	uint8_t start;
	uint8_t mode;
	int16_t pitch;
	int16_t roll;
	int16_t yaw;
    int16_t lift;
	uint8_t stop;	 
};

void print_message_hex(struct message m);


#endif /* MESSAGE_H_ */
