#include "sensor.h"

void trigger_sensor(){
    volatile unsigned char i = 0;
    
    P1OUT |= TRIGGER;
    for (;i<9;i++); // 10us at 1MHz
    P1OUT &= ~TRIGGER;
}
