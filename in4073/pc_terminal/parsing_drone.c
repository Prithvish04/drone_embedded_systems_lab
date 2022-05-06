#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <string.h>
#include "parsing_drone.h"    
#include "recieve_msg.h"
    
       
void print_message_block(int message[],int start_index, int length) 
{
    char buffer[length+1]; 
    for (int i =start_index; i<start_index+length; i++)
    {
        buffer[i-start_index] = message[i];
    } 
    buffer[length] = '\n';
    printf("%lld |",to_dec(buffer,length-1));
}