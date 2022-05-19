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


/**
 * @brief Safe mode handler, set all motor values to zero
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes safeModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    set_motors(0, 0, 0, 0);
    update_motors();
    return Safe_Mode;
}


/**
 * @brief Panikk    
 * slowdown_rpm should be tuned to have smoother landings 
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes panicModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint32_t period_us = 200000;
    static const uint8_t slowdown_rpm = 10;
    static uint32_t deadline = 0;

    if (get_time_us() >= deadline){
        deadline = get_time_us() + period_us;
        if (mode == Panic_Mode){
            if ((motor[0] == 0) && (motor[1] == 0) &&
                (motor[2] == 0) && (motor[3] == 0)) {
                nrf_gpio_pin_set(RED);
                return Safe_Mode;
            }
            if ((motor[0] <= slowdown_rpm + MIN_RPM) || (motor[1] <= slowdown_rpm + MIN_RPM) ||
                (motor[2] <= slowdown_rpm + MIN_RPM) || (motor[3] <= slowdown_rpm + MIN_RPM)) 
                set_motors(0, 0, 0, 0);
            else
                set_motors(ae[0]-slowdown_rpm, ae[1]-slowdown_rpm, ae[2]-slowdown_rpm, ae[3]-slowdown_rpm);
        }
        else{
            nrf_gpio_pin_clear(RED);
            uint16_t max = 0;
            for (uint8_t i = 0; i < 4; ++i){
                if (motor[i] > max)
                    max = motor[i];
            }
            if (max > MAX_RPM)
                set_motors(MAX_RPM, MAX_RPM, MAX_RPM, MAX_RPM);
            else
                set_motors(max, max, max, max);
        }
        update_motors();
    }        
    return Panic_Mode;
}


/**
 * @brief Manual mode
 * Keyboard (a-z, q-w and arrow keys) add static offset to the linear mapping generated by the stick.
 * Values are never allowed to exceed MAX_RPM and fall under MIN_RPM (except for setting to zero)
 * Not sure if only the lift should be capped at MAX_RPM since yaw, pitch and roll controls are also affected otherwise.
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes manualModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint32_t period_us = 2000;
    static const uint8_t offset = 5;
    static const int16_t max_offset = 50;

    static uint32_t deadline = 0;
    static int32_t L = 0, R = 0, P = 0, Y = 0;
    static int16_t L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0;

    // enter manual mode from a different mode
    if (mode != Manual_Mode){
        L = 0; R = 0; P = 0; Y = 0;
        L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0;
        if (!check_neutral(cmd))
            return mode;
    }
    if(cmd->event == Safe_Event) 
        return Panic_Mode;

    // process keyboard inputs
    switch (cmd->key){
    case LIFT_UP_KEY:
        L_offset = (L_offset + offset > max_offset) ? max_offset : L_offset + offset;
        break;
    case LIFT_DOWN_KEY:
        L_offset = (L_offset - offset > -max_offset) ? L_offset - offset : -max_offset;
        break;
    case ROLL_UP_KEY:
        R_offset = (R_offset + offset > max_offset) ? max_offset : R_offset + offset;
        break;
    case ROLL_DOWN_KEY:
        R_offset = (R_offset - offset > -max_offset) ? R_offset - offset : -max_offset;
        break;
    case PITCH_UP_KEY:
        P_offset = (P_offset + offset > max_offset) ? max_offset : P_offset + offset;
        break;
    case PITCH_DOWN_KEY:
        P_offset = (P_offset - offset > -max_offset) ? P_offset - offset : -max_offset;
        break;
    case YAW_UP_KEY:
        Y_offset = (Y_offset + offset > max_offset) ? max_offset : Y_offset + offset;
        break;
    case YAW_DOWN_KEY:
        Y_offset = (Y_offset - offset > -max_offset) ? Y_offset - offset : -max_offset;
        break;
    default:
        break;
    }
    cmd->key = 0xFF;  
    cmd->lift_offset = L_offset;
    cmd->roll_offset = R_offset;
    cmd->pitch_offset = P_offset;
    cmd->yaw_offset = Y_offset;

    // map joystick inputs to rpms 
    R = map_limits(max_offset, -max_offset, -32768, 32767, cmd->roll) + R_offset;
    P = map_limits(max_offset, -max_offset, -32768, 32767, cmd->pitch) + P_offset; 
    Y = map_limits(max_offset, -max_offset, -32768, 32767, cmd->yaw) + Y_offset;
    L = map_limits(MAX_RPM, MIN_RPM, -32768, 32767, -cmd->lift) + L_offset;
    
    if (get_time_us() >= deadline){
        deadline = get_time_us() + period_us;
        if (cmd->lift < 32766)
            set_motors(L-P-Y, L-R+Y, L+P-Y, L+R+Y);
        else
            set_motors(0, 0, 0, 0);
        update_motors();
    }        
    return Manual_Mode;
}

/**
 * @brief Calibration mode
 * Dunno how this is supposed to be implemented, but lets take the mean of the individual euler angles
 * during a certain periodic duration and update calibration offsets at the end of each period.
 * Currently assumed to be reachable only from the safe mode because it doesn't make sense to do otherwise.
 * 
 * Note: the current implementation is not actually the mean, rather a weighted sum of past values where
 * their limit goes to zero as the influence of preceding values decay over time.
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes calibrationModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint8_t save_cnt = 50;
    static const uint32_t period_us = 20000;
    static uint32_t deadline = 0;

    static int32_t phi_mean = 0, theta_mean = 0, psi_mean = 0;
    static uint8_t i = 0;

    // first enterance from a different mode
    if (mode != Calibration_Mode){
        phi_mean = mes->phi, theta_mean = mes->theta, psi_mean = mes->psi;
        i = 0;
    }
    if (get_time_us() >= deadline){
        // calculate offsets
        deadline = get_time_us() + period_us;
        phi_mean += mes->phi; 
        phi_mean /= 2;
        psi_mean += mes->psi;
        psi_mean /= 2;
        theta_mean += mes->theta;
        theta_mean /= 2;
        // save offsets
        if (i++ == save_cnt) {
            mes->phi_offset = phi_mean;
            mes->psi_offset = psi_mean;
            mes->theta_offset = theta_mean;
            phi_mean = mes->phi, theta_mean = mes->theta, psi_mean = mes->psi;
            i = 0;
        }
    }        
    return Calibration_Mode;
}


Modes yawModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    return Yaw_Mode;
}


Modes fullModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    return Full_Mode;
}


Modes rawModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    return Raw_Mode;
}


Modes heightModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    return Height_Mode;
}


Modes wirelessModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    return Wireless_Mode;
}

