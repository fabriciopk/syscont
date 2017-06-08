
#ifndef __sysclean_sensor__
#define __sysclean_sensor__

#include <stdint.h>
#include "msp-cpu.h"
#include "utils.h"

//volatile unsigned char TIMER_FLAG;

#define MIN_VALUE 20
#define MAX_VALUE 150

double read_sensor_cm();
float sensor_get();

#endif
