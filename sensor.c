#include "sensor.h"

unsigned short read_sensor_cm(){
    volatile unsigned char i;
    unsigned long timer;
    unsigned short distance;
    float aux;
    
    P1OUT |= TRIGGER;
    for (i=0;i<9;i++); // ~ 10us at 1MHz
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
    
    aux = (float)timer * 0.01715;
    
    return (unsigned short)aux;
}

float sensor_get(){
    unsigned short measures[5];
    unsigned short maior, menor;
    unsigned int soma;
    unsigned char i;
    
    delay(3000);
    
    for (i=0; i<5; i++){
        measures[i] = read_sensor_cm();
        delay(2000);
    }
    
    maior = measures[0];
    menor = measures[0];
    
    soma = 0;
    
    for (i=1; i<5; i++){
        soma += measures[i];
        
        if (measures[i] > maior) maior = measures[i];
        if (measures[i] < menor) menor = measures[i];
    }
    
    // Remove os dois extremos da media
    soma -= maior;
    soma -= menor;
    
    return ((float)soma/3);
}
