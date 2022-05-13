
#include "send_msg.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


void print_message_hex(struct message m)
{
	printf("%2X ", m.start); //2
	printf("%4X ", m.pitch); // 4
	printf("%4X ", m.roll); //4
	printf("%4X ", m.yaw); //4
	printf("%4X ", m.lift); //4
	printf("%2X ", m.mode); // 2
	printf("%2X \n", m.stop); //2
}
