#ifndef PROFILER_H_
#define PROFILER_H_

#include <inttypes.h>
#include <stdbool.h>
#include "structs.h"


typedef struct time_struct{
    uint32_t starttime;
    uint32_t stoptime;
    char name[10]; 
} time_struct;

#endif /* PROFILER_H_ */
