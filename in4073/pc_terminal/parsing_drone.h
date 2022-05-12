#ifndef PARSING_DRONE_H__
#define PARSING_DRONE_H__

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "recieve_msg.h"

void print_message_block(int message[],int start_index, int length); 

#endif /* PARSING_DRONE_H__*/