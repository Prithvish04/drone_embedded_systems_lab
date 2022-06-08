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
    static const uint8_t offset = 5, d = 100;
    static const int16_t keyboard_lim = 50, joystick_lim = 120;
    static const int16_t lower_Z = 250, upper_Z = 2500;

    static uint32_t deadline = 0;
    static int32_t L = 0, M = 0, N = 0, Z = 0;
    static int16_t L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0;
    static uint32_t m0, m1, m2, m3;

    // enter manual mode from a different mode
    if (mode != Manual_Mode){
        L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0;
        if (!check_neutral(cmd))
             return mode;
    }
    if(cmd->event == Safe_Event) 
        return Panic_Mode;

    // process keyboard inputs
    switch (cmd->key){
    case LIFT_UP_KEY:
        L_offset = (L_offset + offset > keyboard_lim) ? keyboard_lim : L_offset + offset;
        break;
    case LIFT_DOWN_KEY:
        L_offset = (L_offset - offset > -keyboard_lim) ? L_offset - offset : -keyboard_lim;
        break;
    case ROLL_UP_KEY:
        R_offset = (R_offset + offset > keyboard_lim) ? keyboard_lim : R_offset + offset;
        break;
    case ROLL_DOWN_KEY:
        R_offset = (R_offset - offset > -keyboard_lim) ? R_offset - offset : -keyboard_lim;
        break;
    case PITCH_UP_KEY:
        P_offset = (P_offset + offset > keyboard_lim) ? keyboard_lim : P_offset + offset;
        break;
    case PITCH_DOWN_KEY:
        P_offset = (P_offset - offset > -keyboard_lim) ? P_offset - offset : -keyboard_lim;
        break;
    case YAW_UP_KEY:
        Y_offset = (Y_offset + offset > keyboard_lim) ? keyboard_lim : Y_offset + offset;
        break;
    case YAW_DOWN_KEY:
        Y_offset = (Y_offset - offset > -keyboard_lim) ? Y_offset - offset : -keyboard_lim;
        break;
    default:
        break;
    }
    cmd->key = 0xFF;  
    cmd->lift_offset = L_offset;
    cmd->roll_offset = R_offset;
    cmd->pitch_offset = P_offset;
    cmd->yaw_offset = Y_offset;

    
    if (get_time_us() >= deadline){
        deadline = get_time_us() + period_us;
        // map joystick inputs to rpms 
        M = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->roll);
        L = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->pitch); 
        N = map_limits(2*joystick_lim, -2*joystick_lim, -32768, 32767, cmd->yaw);
        Z = map_limits(upper_Z, lower_Z, -32768, 32767, -cmd->lift);
        m0 = sqrt_32(d*(Z + 2*M - N));
        m1 = sqrt_32(d*(Z - 2*L + N));
        m2 = sqrt_32(d*(Z - 2*M - N));
        m3 = sqrt_32(d*(Z + 2*L + N));
        
        if (cmd->lift < 32766)
            set_motors(m0 - R_offset - Y_offset + L_offset, 
                       m1 + P_offset + Y_offset + L_offset,
                       m2 + R_offset - Y_offset + L_offset,
                       m3 - P_offset + Y_offset + L_offset);
        else
            set_motors(0, 0, 0, 0);
        update_motors();
    }        
    return Manual_Mode;
}

/**
 * @brief Calibration mode
 * 5 secs for yaw pitch roll averaging and saving them as offsets
 * goes back to safe mode at the end automatically
 * you can interrupt this by going to a different mode manually
 * 
 * raw mode calibration ¯\_(ツ)_/¯
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes calibrationModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint16_t save_cnt = 5000;
    static const uint32_t period_us = 1000;
    static uint32_t deadline = 0;

    static int32_t phi_mean = 0, theta_mean = 0, psi_mean = 0;
    static int32_t sp_mean = 0, sq_mean = 0, sr_mean = 0;
    static int32_t sax_mean = 0, say_mean = 0, saz_mean = 0;
    static uint16_t i = 0;

    // first enterance from a different mode
    if ((mode != Calibration_Mode) || (cmd->event != Null)){
        phi_mean = 0, theta_mean = 0, psi_mean = 0;
        sp_mean = 0,  sq_mean = 0,    sr_mean = 0;
        sax_mean = 0, say_mean = 0,   saz_mean = 0;
        i = 0;
    }
    
    if (get_time_us() >= deadline){
        // calculate offsets
        deadline = get_time_us() + period_us;
        phi_mean += mes->phi;  psi_mean += mes->psi; theta_mean += mes->theta;
        sp_mean += mes->sp;    sq_mean += mes->sq;   sr_mean += mes->sr;
        sax_mean += mes->sax;  say_mean += mes->say; saz_mean += mes->saz;
        // save offsets
        if (i++ == save_cnt) {
            mes->phi_offset = phi_mean/i; mes->psi_offset = psi_mean/i;  mes->theta_offset = theta_mean/i;
            mes->sp_offset = sp_mean/i;   mes->sq_offset = sq_mean/i;    mes->sr_offset = sr_mean/i;
            mes->sax_offset = sax_mean/i; mes->say_offset = say_mean/i;  mes->saz_offset = saz_mean/i; 
            phi_mean = 0, theta_mean = 0, psi_mean = 0;
            sp_mean = 0,  sq_mean = 0,    sr_mean = 0;
            sax_mean = 0, say_mean = 0,   saz_mean = 0;
            i = 0;
            return Safe_Mode;
        }
    }        
    return Calibration_Mode;
}

/**
 * @brief Yaw Control (more like yaw rate control)
 * max_r, initial K and Sc should be tuned 
 * uncomment the print line while commenting out the GUI for debugging
 * 
 * @param mode 
 * @param cmd 
 * @param mes 
 * @return Modes 
 */
Modes yawModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint8_t offset = 5, d = 100, k_offset = 1;
    //static const int16_t keyboard_lim = 50, joystick_lim = 120, Sc = 512;
    static const int16_t keyboard_lim = 50, joystick_lim = 120;
    static const int16_t lower_Z = 250, upper_Z = 2500, K_init = 0;

    static uint32_t deadline = 0;
    static int32_t L = 0, M = 0, N = 0, Z = 0;
    static int16_t L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0, G_offset = 0;
    static uint32_t m0, m1, m2, m3;


    // enter Yaw mode from a different mode
    if (mode != Yaw_Mode){
        L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0, G_offset = 0;
        G_offset = K_init;
        // cmd->P1 = K_init;
        // cmd->P2 = K_init;
        if (!check_neutral(cmd))
             return mode;
    }
    if(cmd->event == Safe_Event) 
        return Panic_Mode;

    // process keyboard inputs
    switch (cmd->key){
    case LIFT_UP_KEY:
        L_offset = (L_offset + offset > keyboard_lim) ? keyboard_lim : L_offset + offset;
        break;
    case LIFT_DOWN_KEY:
        L_offset = (L_offset - offset > -keyboard_lim) ? L_offset - offset : -keyboard_lim;
        break;
    case ROLL_UP_KEY:
        R_offset = (R_offset + offset > keyboard_lim) ? keyboard_lim : R_offset + offset;
        break;
    case ROLL_DOWN_KEY:
        R_offset = (R_offset - offset > -keyboard_lim) ? R_offset - offset : -keyboard_lim;
        break;
    case PITCH_UP_KEY:
        P_offset = (P_offset + offset > keyboard_lim) ? keyboard_lim : P_offset + offset;
        break;
    case PITCH_DOWN_KEY:
        P_offset = (P_offset - offset > -keyboard_lim) ? P_offset - offset : -keyboard_lim;
        break;
    case YAW_UP_KEY:
        Y_offset = (Y_offset + offset > keyboard_lim) ? keyboard_lim : Y_offset + offset;
        break;
    case YAW_DOWN_KEY:
        Y_offset = (Y_offset - offset > -keyboard_lim) ? Y_offset - offset : -keyboard_lim;
        break;
    case P_UP_KEY: 
        G_offset += k_offset; 
        break;
    case P_DOWN_KEY:
        G_offset = (G_offset - k_offset < 0) ? 0 : G_offset - k_offset;
        break;
    default:
        break;
    }
    cmd->key = 0xFF;  

    //Same as Manual Mode
    cmd->lift_offset = L_offset;
    cmd->roll_offset = R_offset;
    cmd->pitch_offset = P_offset;
    cmd->yaw_offset = Y_offset;
    cmd->P = G_offset;

    if (get_time_us() >= deadline){
         // map joystick inputs to rpms 
        M = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->roll);
        L = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->pitch); 
        N = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->yaw);
        Z = map_limits(upper_Z, lower_Z, -32768, 32767, -cmd->lift);

        // cap the readings to the allowed values set by max_r
        mes->sr = ((mes->sr < -joystick_lim) ? -joystick_lim : ((mes->sr > joystick_lim) ? joystick_lim : mes->sr));
        // mes->phi = ((mes->phi < -joystick_lim) ? -joystick_lim : ((mes->phi > joystick_lim) ? joystick_lim : mes->phi));
        // mes->theta = ((mes->theta < -joystick_lim) ? -joystick_lim : ((mes->theta > joystick_lim) ? joystick_lim : mes->theta));
        // error wrt setpoint from the joystick 
        N = G_offset * (N - (int32_t) mes->sr);
        // r += error;
        // r /= Sc;
        // Y = r + Y_offset;
    
        // M += cmd->P1 * (M - (int32_t) mes->phi);
        // L += cmd->P2 * (L - (int32_t) mes->theta); 

        
        m0 = sqrt_32(d*(Z + 2*M) - N);
        m1 = sqrt_32(d*(Z - 2*L) + N);
        m2 = sqrt_32(d*(Z - 2*M) - N);
        m3 = sqrt_32(d*(Z + 2*L) + N);
        
        if (cmd->lift < 32766)
            set_motors(m0 - R_offset - Y_offset + L_offset, 
                       m1 + P_offset + Y_offset + L_offset,
                       m2 + R_offset - Y_offset + L_offset,
                       m3 - P_offset + Y_offset + L_offset);
        else
            set_motors(0, 0, 0, 0);
        update_motors();



        // deadline = get_time_us() + period_us;
        // // map joystick inputs to rpms (Same as Manual Mode)
        // R = map_limits(max_offset, -max_offset, -32768, 32767, cmd->roll) + R_offset;
        // P = map_limits(max_offset, -max_offset, -32768, 32767, cmd->pitch) + P_offset; 
        // L = map_limits(MAX_RPM, MIN_RPM, -32768, 32767, -cmd->lift) + L_offset;
        // // map yaw axis to yaw rate ¯\_(ツ)_/¯
        // r = map_limits(max_r, -max_r, -32768, 32767, cmd->yaw); 
        // // cap the readings to the allowed values set by max_r
        // mes->sr = ((mes->sr < -max_r) ? -max_r : ((mes->sr > max_r) ? max_r : mes->sr));
        // // error wrt setpoint from the joystick 
        // error = K*(r - (int32_t) mes->sr);
        // r += error;
        // r /= Sc;
        // Y = r + Y_offset;

        // //printf("debug m0: %d m1: %d m2: %d m3: %d R: %ld P: %ld L: %ld N: %ld r: %d e: %ld \n", ae[0], ae[1], ae[2], ae[3], R, P, L, N, mes->sr, error);
        // if (cmd->lift < 32766)
        //     set_motors(L-R-Y, L+P+Y, L+R-Y, L-P+Y);
        // else
        //     set_motors(0, 0, 0, 0);
        // update_motors();
    }        
    return Yaw_Mode;
}


Modes fullModeHandler(Modes mode, DroneMessage* cmd, Measurement* mes){
    static const uint8_t offset = 5, k_offset = 1, d = 100;
    //static const int16_t keyboard_lim = 50, joystick_lim = 120, Sc = 512;
    static const int16_t keyboard_lim = 50, joystick_lim = 120;
    static const int16_t lower_Z = 250, upper_Z = 2500, K_init = 0;

    static uint32_t deadline = 0;
    static int32_t L = 0, M = 0, N = 0, Z = 0;
    static int16_t L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0, G_offset = 0, G1_offset = 0, G2_offset = 0;
    static uint32_t m0, m1, m2, m3;


    // enter Yaw mode from a different mode
    if (mode != Full_Mode){
        L_offset = 0, R_offset = 0, P_offset = 0, Y_offset = 0;
        G_offset = K_init;
        G1_offset = K_init;
        G2_offset = K_init;
        if (!check_neutral(cmd))
             return mode;
    }
    if(cmd->event == Safe_Event) 
        return Panic_Mode;

    // process keyboard inputs
    switch (cmd->key){
    case LIFT_UP_KEY:
        L_offset = (L_offset + offset > keyboard_lim) ? keyboard_lim : L_offset + offset;
        break;
    case LIFT_DOWN_KEY:
        L_offset = (L_offset - offset > -keyboard_lim) ? L_offset - offset : -keyboard_lim;
        break;
    case ROLL_UP_KEY:
        R_offset = (R_offset + offset > keyboard_lim) ? keyboard_lim : R_offset + offset;
        break;
    case ROLL_DOWN_KEY:
        R_offset = (R_offset - offset > -keyboard_lim) ? R_offset - offset : -keyboard_lim;
        break;
    case PITCH_UP_KEY:
        P_offset = (P_offset + offset > keyboard_lim) ? keyboard_lim : P_offset + offset;
        break;
    case PITCH_DOWN_KEY:
        P_offset = (P_offset - offset > -keyboard_lim) ? P_offset - offset : -keyboard_lim;
        break;
    case YAW_UP_KEY:
        Y_offset = (Y_offset + offset > keyboard_lim) ? keyboard_lim : Y_offset + offset;
        break;
    case YAW_DOWN_KEY:
        Y_offset = (Y_offset - offset > -keyboard_lim) ? Y_offset - offset : -keyboard_lim;
        break;
    case P_UP_KEY: 
        G_offset += k_offset; 
        break;
    case P_DOWN_KEY:
        G_offset = (G_offset - k_offset < 0) ? 0 : G_offset - k_offset;
        break;
    case P1_UP_KEY: 
        G1_offset += k_offset;  
        break;        
    case P1_DOWN_KEY:
        G1_offset = (G1_offset - k_offset < 0) ? 0 : G1_offset - k_offset;
        break;
    case P2_UP_KEY: 
        G2_offset += k_offset; 
        break;        
    case P2_DOWN_KEY:
        G2_offset = (G2_offset - k_offset < 0) ? 0 : G2_offset - k_offset;
        break;
    default:
        break;
    }
    cmd->key = 0xFF;  

    //Same as Manual Mode
    cmd->lift_offset = L_offset;
    cmd->roll_offset = R_offset;
    cmd->pitch_offset = P_offset;
    cmd->yaw_offset = Y_offset;
    cmd->P = G_offset;
    cmd->P1 = G1_offset;
    cmd->P2 = G2_offset;

    if (get_time_us() >= deadline){
         // map joystick inputs to rpms 
        M = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->roll);
        L = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->pitch); 
        N = map_limits(joystick_lim, -joystick_lim, -32768, 32767, cmd->yaw);
        Z = map_limits(upper_Z, lower_Z, -32768, 32767, -cmd->lift);

        // cap the readings to the allowed values set by max_r
        mes->sr = ((mes->sr < -joystick_lim) ? -joystick_lim : ((mes->sr > joystick_lim) ? joystick_lim : mes->sr));
        mes->sq = ((mes->sq < -joystick_lim) ? -joystick_lim : ((mes->sq > joystick_lim) ? joystick_lim : mes->sq));
        mes->sp = ((mes->sp < -joystick_lim) ? -joystick_lim : ((mes->sp > joystick_lim) ? joystick_lim : mes->sp));
        mes->phi = ((mes->phi < -joystick_lim) ? -joystick_lim : ((mes->phi > joystick_lim) ? joystick_lim : mes->phi));
        mes->theta = ((mes->theta < -joystick_lim) ? -joystick_lim : ((mes->theta > joystick_lim) ? joystick_lim : mes->theta));
        // error wrt setpoint from the joystick 
        N = G_offset * (N - (int32_t) mes->sr);
        // r += error;
        // r /= Sc;
        // Y = r + Y_offset;
    
        L = G1_offset * (L - (int32_t) mes->theta);
        L = G2_offset * (L - (int32_t) mes->sq);

        //L = G1_offset * (L - (int32_t) mes->phi);
        //L = G2_offset * (L - (int32_t) mes->sp);

        m0 = sqrt_32(d*(Z) + 2*M - N);
        m1 = sqrt_32(d*(Z) - 2*L + N);
        m2 = sqrt_32(d*(Z) - 2*M - N);
        m3 = sqrt_32(d*(Z) + 2*L + N);
        
        if (cmd->lift < 32766)
            set_motors(m0 - R_offset - Y_offset + L_offset, 
                       m1 + P_offset + Y_offset + L_offset,
                       m2 + R_offset - Y_offset + L_offset,
                       m3 - P_offset + Y_offset + L_offset);
        else
            set_motors(0, 0, 0, 0);
        update_motors();



        // deadline = get_time_us() + period_us;
        // // map joystick inputs to rpms (Same as Manual Mode)
        // R = map_limits(max_offset, -max_offset, -32768, 32767, cmd->roll) + R_offset;
        // P = map_limits(max_offset, -max_offset, -32768, 32767, cmd->pitch) + P_offset; 
        // L = map_limits(MAX_RPM, MIN_RPM, -32768, 32767, -cmd->lift) + L_offset;
        // // map yaw axis to yaw rate ¯\_(ツ)_/¯
        // r = map_limits(max_r, -max_r, -32768, 32767, cmd->yaw); 
        // // cap the readings to the allowed values set by max_r
        // mes->sr = ((mes->sr < -max_r) ? -max_r : ((mes->sr > max_r) ? max_r : mes->sr));
        // // error wrt setpoint from the joystick 
        // error = K*(r - (int32_t) mes->sr);
        // r += error;
        // r /= Sc;
        // Y = r + Y_offset;

        // //printf("debug m0: %d m1: %d m2: %d m3: %d R: %ld P: %ld L: %ld N: %ld r: %d e: %ld \n", ae[0], ae[1], ae[2], ae[3], R, P, L, N, mes->sr, error);
        // if (cmd->lift < 32766)
        //     set_motors(L-R-Y, L+P+Y, L+R-Y, L-P+Y);
        // else
        //     set_motors(0, 0, 0, 0);
        // update_motors();
    }        
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
