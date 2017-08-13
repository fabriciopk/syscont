#include "sensor.h"

double read_sensor_cm(){
    volatile uint8_t i;
    uint16_t timer;
    double aux;

    P2OUT |= TRIGGER;
    // ~ 10us at 1MHz
    i = 0;
    i = 1;
    P2OUT &= ~TRIGGER;

    TA0CTL = TACLR;
    TA0CTL = TASSEL_2 + MC_2 + ID_3; // SMCLK/8, upmode

    while(! (P1IN & ECHO)){ // wait for echo to go UP
        if (TA0R > 1250){ // no echo received in 10 ms
            TA0CTL = TACLR;
            return 0;
        }
    }
    TA0CTL = TACLR;

    TA0CTL = TASSEL_2 + MC_2 + ID_0; // SMCLK/1, upmode

    while (P1IN & ECHO){
        if (TA0R > 50000){ // echo up more than 50ms
            TA0CTL = TACLR;
            return 0;
        }
    }

    timer = TA0R;
    TA0CTL = MC_0;
    TA0CTL = TACLR;

    aux = (double)timer * 0.01715; // cm / us

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

        if (count >= 15) break; // i >= 4
        count++;

        delay(1000);
    }

    if (count >= 15){
        return 0;
    }

    return (soma/5);
}
