
#ifndef __sysclean_sensor__
#define __sysclean_sensor__

#include <stdint.h>
#include "msp-cpu.h"
#include "utils.h"

//volatile unsigned char TIMER_FLAG;

#define MIN_VALUE 10
#define MAX_VALUE 254

typedef struct{
    uint8_t trigger;
    uint8_t echo;
} sensor_ports;

typedef enum{
    SENSOR1 = 0,
    SENSOR2
} sensor_t;

double read_sensor_cm(sensor_t);
float sensor_get(sensor_t);

#endif
