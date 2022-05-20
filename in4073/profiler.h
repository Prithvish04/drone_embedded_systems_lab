#ifndef PROFILER_H_
#define PROFILER_H_

#include <inttypes.h>
#include <stdbool.h>
#include "structs.h"


typedef struct time_struct{
    uint32_t starttime;
    uint32_t stoptime;
} time_struct;

void getstarttime(time_struct  mytimer);

void getstoptime(time_struct mytimer);

uint32_t timedoff(time_struct mytimer);

#endif /* PROFILER_H_ */
