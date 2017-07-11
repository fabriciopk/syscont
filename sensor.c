#include "sensor.h"

double read_sensor_cm(sensor_t s){
    volatile uint8_t i;
    uint16_t timer;
    double aux;
    sensor_ports sensor;
    
    sensor.pout = P2OUT;
#ifdef TWO_SENSORS
    if (s == SENSOR1){
#endif
        sensor.trigger = TRIGGER;
        sensor.pin = P1IN;
        sensor.echo = ECHO;
#ifdef TWO_SENSORS
    } else{
        sensor.trigger = TRIGGER2;
        sensor.pin = P2IN;
        sensor.echo = ECHO2;
    }
#endif
    
    sensor.pout |= sensor.trigger;
    // ~ 10us at 1MHz
    i = 0;
    i = 1;
    sensor.pout &= ~sensor.trigger;
    
    TA0CTL = TACLR;
    while(! (sensor.pin & sensor.echo)); // wait for echo to go UP
    
    TA0CTL = TASSEL_2 + MC_2 + ID_0; // SMCLK/1, upmode
#ifdef DEBUG
    ledOn(RED_LED);
#endif
    
    while (sensor.pin & sensor.echo);
    
    timer = TA0R;
    TA0CTL = MC_0;
#ifdef DEBUG
    ledOff(RED_LED);
#endif
    TA0CTL = TACLR;
    
    aux = (double)timer * 0.01715;
    
    return aux;
}

float sensor_get(sensor_t s){
    double measures[5];
    double maior, menor;
    double soma;
    uint8_t i;
    uint8_t count = 0;
    
    delay(3000);
    
    soma = 0;
    
    for (i=0; i<5; i++){
        measures[i] = read_sensor_cm(s);
        
        if (measures[i] < MIN_VALUE || measures[i] > MAX_VALUE){
            i--;
        } else{
            soma += measures[i];
        }
        
        if (count >= 10) break; // i >= 4
        count++;
        
        delay(1000);
    }
    
    if (count >= 10){
        return 0;
    }
    
    return (soma/5);
}
