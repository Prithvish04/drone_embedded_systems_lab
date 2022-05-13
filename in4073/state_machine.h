

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_


#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "parse.h"

enum Mode {
    Safe = 0,
    Panic = 1,
    Manual = 2,
    Calibration = 3,
    YawControl = 4,
    FullControl = 5,
    Raw = 6,
    HeightControl = 7,
    Wireless = 8,
};

void state_machine(struct drone_message dmsg);
void change_mode(const enum Mode changed_mode);


#endif /* STATE_MACHINE_H_ */

