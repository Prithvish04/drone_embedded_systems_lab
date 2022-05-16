
#include "send_msg.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


void print_message(struct message m)
{
	printf("%2X ", m.start); //2
	printf("%d ", (int16_t)((m.pitch_H << 8) | m.pitch_L)); // 4
	printf("%d ", (int16_t)((m.roll_H << 8) | m.roll_L)); //4
	printf("%d ", (int16_t)((m.yaw_H << 8) | m.yaw_L)); //4
	printf("%d ", (int16_t)((m.lift_H << 8) | m.lift_L)); //4
	printf("%2X ", m.mode); // 2
	printf("%2X \n", m.stop); //2
}
