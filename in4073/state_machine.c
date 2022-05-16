#include "state_machine.h"

/**
 * Important note, dont put any delays or overarching loops within these implementations.
 * They should only implement one cycle of their corresponding algorithms, since they will be
 * called continuously from the main loop anyway. Thats why we keep the Null event in the FSM table.
 * Null event always points to the current function so that it is repeatedly called when we are not changing the modes.
 * Use static variables to save a current value for using in the succeeding function call. We might have to modify
 * the parameters tho, not sure about that yet (Especially for saving calibration values).
 * Also dont put any function other than a handler here to keep things simple.
 */


Modes safeModeHandler(DroneMessage* cmd, Measurement* mes){
    shutdown_motors();
    return Safe_Mode;
}

Modes panicModeHandler(DroneMessage* cmd, Measurement* mes){
    static uint16_t land_duration_ms = 3000;
    nrf_gpio_pin_set(RED);
    
    /*
    nrf_gpio_pin_clear(RED);
    all_motors_grad_slow();
    all_motors_hold_speed(us_2);
    all_motors_stop();
    nrf_gpio_pin_set(RED);
    printf("Landed in Panic Mode!");
    change_mode(Safe);
    */
    return Safe_Mode;
}

Modes manualModeHandler(DroneMessage* cmd, Measurement* mes){
    return Manual_Mode;
}

Modes calibrationModeHandler(DroneMessage* cmd, Measurement* mes){
    return Calibration_Mode;
}

Modes yawModeHandler(DroneMessage* cmd, Measurement* mes){
    return Yaw_Mode;
}

Modes fullModeHandler(DroneMessage* cmd, Measurement* mes){
    return Full_Mode;
}

Modes rawModeHandler(DroneMessage* cmd, Measurement* mes){
    return Raw_Mode;
}

Modes heightModeHandler(DroneMessage* cmd, Measurement* mes){
    return Height_Mode;
}

Modes wirelessModeHandler(DroneMessage* cmd, Measurement* mes){
    return Wireless_Mode;
}

