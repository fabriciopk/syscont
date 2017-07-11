
#ifndef __sysclean_sensor__
#define __sysclean_sensor__

#include <stdint.h>
#include "msp-cpu.h"
#include "utils.h"

//volatile unsigned char TIMER_FLAG;

#define MIN_VALUE 20
#define MAX_VALUE 150

typedef struct{
    uint8_t pout;
    uint16_t trigger;
    uint8_t pin;
    uint16_t echo;
} sensor_ports;

typedef enum{
    SENSOR1 = 0,
#ifdef TWO_SENSORS
    SENSOR2
#endif
} sensor_t;

double read_sensor_cm(sensor_t);
float sensor_get(sensor_t);

#endif
