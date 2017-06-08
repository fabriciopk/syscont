#include "sensor.h"

double read_sensor_cm(){
    volatile uint8_t i;
    uint16_t timer;
    double aux;
    
    P1OUT |= TRIGGER;
    // ~ 10us at 1MHz
    i = 0;
    i = 1;
    P1OUT &= ~TRIGGER;
    
    TA0CTL = TACLR;
    while(! (P1IN & ECHO)); // wait for echo to go UP
    
    TA0CTL = TASSEL_2 + MC_2 + ID_0; // SMCLK/1, upmode
#ifdef DEBUG
    ledOn(RED_LED);
#endif
    
    while (P1IN & ECHO);
    
    timer = TA0R;
    TA0CTL = MC_0;
#ifdef DEBUG
    ledOff(RED_LED);
#endif
    TA0CTL = TACLR;
    
    aux = (double)timer * 0.01715;
    
    return aux;
}

float sensor_get(){
    double measures[5];
    double maior, menor;
    double soma;
    uint8_t i;
    uint8_t count = 0;
    
    delay(3000);
    
    soma = 0;
    
    for (i=0; i<5; i++){
        measures[i] = read_sensor_cm();
        
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
