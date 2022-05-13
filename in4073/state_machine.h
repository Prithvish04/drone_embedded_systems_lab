
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
#include "parse.h"

/**
static const uint32_t us_1_2 = (500ULL*1000ULL);
static const uint32_t us_1 = (2ULL * us_1_2);
static const uint32_t us_2 = (2ULL * us_1);
static const uint32_t critical_time = (1ULL * us_1_2);
*//

typedef enum {Safe_Mode,
              Panic_Mode,
              Manual_Mode,
              Calibration_Mode,
              Yaw_Mode,
              Full_Mode,
              Raw_Mode,
              Height_Mode,
              Wireless_Mode,
              __last_Mode} Modes;

typedef enum {Exit,
              Safe_Event,
              Panic_Event,
              Manual_Event,
              Calibration_Event,
              Yaw_Event,
              Full_Event,
              Raw_Event,
              Height_Event,
              Wireless_Event,
              __last_Event} Events;

//typedef of 2d array
typedef Modes (*const EventHandler[__last_Mode][__last_Event])(void);
typedef Modes (*pfEventHandler)(void);

// FSM handlers
Modes safeModeHandler(void);
Modes panicModeHandler(void);
Modes manualModeHandler(void);
Modes calibrationModeHandler(void);
Modes yawModeHandler(void);
Modes fullModeHandler(void);
Modes rawModeHandler(void);
Modes heightModeHandler(void);
Modes wirelessModeHandler(void);
Modes exitHandler(void);

static Modes nextState = Safe_Mode;
static Events newEvent;

// Table to define valid states and event of finite state machine
static afEventHandler StateMachine = {
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
                    [Exit] = exitHandler},
    [Panic_Mode] = {
                    [Panic_Event] = panicModeHandler},
    [Manual_Mode] = {
                    [Manual_Event] = manualModeHandler,
                    [Safe_Event] = safeModeHandler,
                    [Panic_Event] = panicModeHandler}
};


Modes read_event(struct *drone_message);

#endif // MODES_H__


