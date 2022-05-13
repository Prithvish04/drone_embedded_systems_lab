

#include "state_machine.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h> 
#include <stdbool.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "gpio.h"
#include "parse.h"



static enum Mode current_state = Safe; 

static const uint32_t us_1_2 = (500ULL*1000ULL);
static const uint32_t us_1 = (2ULL * us_1_2);
static const uint32_t us_2 = (2ULL * us_1);
static const uint32_t critical_time = (1ULL * us_1_2);

static void panic_handler() {
    nrf_gpio_pin_clear(RED);
    all_motors_grad_slow();
    all_motors_hold_speed(us_2);
    all_motors_stop();
    nrf_gpio_pin_set(RED);
    printf("Landed in Panic Mode!");
    change_mode(Safe);
}    


bool check_panic(struct drone_message dmsg){
    if(dmsg.mode == 31 || dmsg.mode == 27 ){
        return true;
    }
    return false;
}

void change_mode(const enum Mode changed_mode)
{
    current_state = changed_mode;
} 


void state_machine(struct drone_message dmsg)
{
    // why do we need time here ?
    // uint32_t current_time = get_time_us();
    switch(current_state){
        case Safe:
            printf("In safe mode\n");
            if(check_panic(dmsg) == true){
                change_mode(Panic);
            }
            break;
        
        case Panic:
            panic_handler();
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