
#ifndef MODES_H__
#define MODES_H__

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "gpio.h"
#include "control.h"
#include "structs.h"

// static const uint32_t us_1_2 = (500ULL*1000ULL);
// static const uint32_t us_1 = (2ULL * us_1_2);
// static const uint32_t us_2 = (2ULL * us_1);
// static const uint32_t critical_time = (1ULL * us_1_2);



//typedef of 2d array
typedef Modes (*const EventHandler[__last_Mode][__last_Event])(DroneMessage*, Measurement*);
typedef Modes (*pfEventHandler)(void);

// FSM handlers, parameters not sure yet, but they should all be the same 
Modes safeModeHandler(DroneMessage*, Measurement*);
Modes panicModeHandler(DroneMessage*, Measurement*);
Modes manualModeHandler(DroneMessage*, Measurement*);
Modes calibrationModeHandler(DroneMessage*, Measurement*);
Modes yawModeHandler(DroneMessage*, Measurement*);
Modes fullModeHandler(DroneMessage*, Measurement*);
Modes rawModeHandler(DroneMessage*, Measurement*);
Modes heightModeHandler(DroneMessage*, Measurement*);
Modes wirelessModeHandler(DroneMessage*, Measurement*);

// Table to define valid states and event of finite state machine
// Null should always be there, handlers should only execute one cycle of their algorithm
// remove unallowed actions while implementing corresponding modes
// Implemented (or WIP) : Safe, Panic, Manual, Calibration (not sure about this one yet)

static EventHandler StateMachine = {
    [Safe_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Null] = safeModeHandler},

    [Panic_Mode] = {
                    [Panic_Event] = panicModeHandler,
                    [Null] = panicModeHandler},

    [Manual_Mode] = {
                    [Safe_Event] = manualModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Null] = manualModeHandler},

    [Calibration_Mode] = {
                    [Safe_Event] = calibrationModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Null] = calibrationModeHandler},

    // ===========================================================


    [Yaw_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler},

    [Full_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler},

    [Raw_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler},
    [Height_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler},
                    
    [Wireless_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler},
};

#endif // MODES_H__


