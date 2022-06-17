
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
#include "timers.h"
#include "parse.h"


//typedef of 2d array
typedef Modes (*const EventHandler[__last_Mode][__last_Event])(Modes, DroneMessage*, Measurement*);
typedef Modes (*pfEventHandler)(void);

// FSM handlers, parameters not sure yet, but they should all be the same 
Modes safeModeHandler(Modes, DroneMessage*, Measurement*);
Modes panicModeHandler(Modes, DroneMessage*, Measurement*);
Modes manualModeHandler(Modes, DroneMessage*, Measurement*);
Modes calibrationModeHandler(Modes, DroneMessage*, Measurement*);
Modes yawModeHandler(Modes, DroneMessage*, Measurement*);
Modes fullModeHandler(Modes, DroneMessage*, Measurement*);
Modes rawModeHandler(Modes, DroneMessage*, Measurement*);
Modes heightModeHandler(Modes, DroneMessage*, Measurement*);
Modes wirelessModeHandler(Modes, DroneMessage*, Measurement*);

// Table to define valid states and event of finite state machine
// Null should always be there, handlers should only execute one cycle of their algorithm
// remove unallowed actions while implementing corresponding modes

static EventHandler StateMachine = {
    [Safe_Mode] = {
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
                    [Null] = panicModeHandler},

    [Manual_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Null] = manualModeHandler},

    [Calibration_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Null] = calibrationModeHandler},

    [Yaw_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Null] = yawModeHandler},


    [Full_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Null] = fullModeHandler},

    [Raw_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Null] = rawModeHandler},

    [Height_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Height_Event] = heightModeHandler,
                    [Null] = heightModeHandler},
                    
    [Wireless_Mode] = {
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler,
                    [Manual_Event] = manualModeHandler,
                    [Calibration_Event] = calibrationModeHandler,
                    [Yaw_Event] = yawModeHandler,
                    [Full_Event] = fullModeHandler,
                    [Raw_Event] = rawModeHandler, 
                    [Height_Event] = heightModeHandler,
                    [Wireless_Event] = wirelessModeHandler,
                    [Null] = wirelessModeHandler},
};

#endif // MODES_H__


