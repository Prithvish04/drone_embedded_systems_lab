
#include "state_machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h> 

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "gpio.h"



static const uint32_t us_1_2 = (500ULL*1000ULL);
static const uint32_t us_1 = (2ULL * us_1_2);
static const uint32_t us_2 = (2ULL * us_1);
static const uint32_t critical_time = (1ULL * us_1_2);

void state_machine()
{
    uint32_t current_time = get_time_us();
    switch(current_state){
        case Safe:
            printf("In safe mode\n");
            break;
        
        case Panic:
            //panic_handler();
            //change_mode(Safe);
            break; 
        
        case Manual:
            //manual_loop();
            //change_mode(Safe);
            break;

        case Calibration:
            //calibrate();
            //change_mode(Safe);
            break;
        case YawControl:
            //yc_loop();
            //change_mode(Safe);
            break;
        case FullControl:
            //fc_loop();
            //change_mode(Safe);
            break;
        case Raw:
            //raw_loop();
            //change_mode(Safe);
            break;
        case HeightControl:
            //hc_loop();
            //change_mode(Safe);
            break;
        case Wireless:
            //wireless_loop();
            //change_mode(Safe);
            break;
    };
}