
#ifndef __sysclean_sensor__
#define __sysclean_sensor__

#include "utils.h"
#include <stdint.h>

//volatile unsigned char TIMER_FLAG;

#define MIN_VALUE 20
#define MAX_VALUE 150

double read_sensor_cm();
double sensor_get();

#endif
