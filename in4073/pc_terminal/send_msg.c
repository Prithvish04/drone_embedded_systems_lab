
#include "send_msg.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>






void print_message_hex(struct message m)
{
	printf("%02X", m.start); //2
	printf("%04X", m.pitch); // 4
	printf("%04X", m.roll); //4
	printf("%04X", m.yaw); //4
	printf("%04X", m.lift); //4
	printf("%02X", m.mode); // 2
	printf("%02X", m.stop); //2
}
