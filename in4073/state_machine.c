#include "state_machine.h"


Modes read_event(struct *drone_message) {
    static Modes prev = Safe_Mode;
    
}


Modes safeModeHandler(void){
    return Safe_Mode;
}

Modes panicModeHandler(void){
    nrf_gpio_pin_clear(RED);
    all_motors_grad_slow();
    all_motors_hold_speed(us_2);
    all_motors_stop();
    nrf_gpio_pin_set(RED);
    printf("Landed in Panic Mode!");
    change_mode(Safe);
    return Safe_Mode;
}

Modes manualModeHandler(void){
    return Safe_Mode;
}

Modes calibrationModeHandler(void){
    return Safe_Mode;
}

Modes yawModeHandler(void){
    return Safe_Mode;
}

Modes fullModeHandler(void){
    return Safe_Mode;
}

Modes rawModeHandler(void){
    return Safe_Mode;
}

Modes heightModeHandler(void){
    return Safe_Mode;
}

Modes wirelessModeHandler(void){
    return Safe_Mode;
}

Modes exitHandler(void){
    return Safe_Mode;
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