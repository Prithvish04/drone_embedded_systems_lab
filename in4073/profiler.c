

#include "profiler.h"
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <timer.h>


void getstarttime(time_struct  mytimer){
    mytimer.starttime = get_time_us()
}

void getstoptime(time_struct mytimer){
    mytimer.stoptime = get_time_us()
}

uint32_t timedoff(time_struct mytimer){
    return mytimer.stoptime - mytimer.starttime
}
