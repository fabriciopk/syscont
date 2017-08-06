#include <msp430.h>
#include <stdint.h>
#include "msp-cpu.h"
#include "utils.h"
#include "sensor.h"
#include "gprs.h"

/*
 * main.c
 */

// #define TEMPO_SLEEP 10 // 5 seg
// #define TEMPO_SLEEP 20 // 10 seg
// #define TEMPO_SLEEP 40 // 20 seg
#define TEMPO_SLEEP 120 // 1 min
// #define TEMPO_SLEEP 240 // 2 min
// #define TEMPO_SLEEP 600 // 5 min
// #define TEMPO_SLEEP 1200 // 10 min
// #define TEMPO_SLEEP 28800 // 4 hrs

volatile static uint32_t i;
volatile float distance1;
volatile float distance2;
volatile float battery;
volatile uint8_t counter;

void configureIntTimer(void);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    i = 0;
    msp_init();
    read_battery();

    boardOn();
    delay(2000);

    distance1 = sensor_get(SENSOR1);
    distance2 = sensor_get(SENSOR2);
    battery = read_battery();
    battery += read_battery();
    battery += read_battery();
    battery += read_battery();
    battery = battery/4;

    counter = 0;
    do {
        if (counter++ >= 3) break;
        gprs_powerCycle();
        gprs_init();
    } while (!gprs_send_data(distance1, distance2, battery));

    boardOff();

    configureIntTimer();

    LPM0; // LOW POWER MODE 0
    //LPM0_EXIT;

    while(1);
}

void configureIntTimer(void){
    TACTL = TACLR;
    TACCR0 =  62500;                            // 500ms
    TACTL = TASSEL_2 + MC_1 + ID_3;           // SMCLK/8, upmode
    TACCTL0 = CCIE;                             // CCR0 interrupt enabled
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{

    if (i++ >= TEMPO_SLEEP){
        TACCTL0 &= ~CCIE;                             // CCR0 interrupt disabled
        __bis_SR_register(GIE); // enable global interrupt flag

        boardOn();
        delay(2000);

        distance1 = sensor_get(SENSOR1);
        distance2 = sensor_get(SENSOR2);
        battery = read_battery();
        battery += read_battery();
        battery += read_battery();
        battery += read_battery();
        battery = battery/4;

        counter = 0;
        do {
            if (counter++ >= 3) break;
            gprs_powerCycle();
            gprs_init();
        } while (!gprs_send_data(distance1, distance2, battery));

        boardOff();

        i = 0;
        __bic_SR_register(GIE); // disable global interrupt flag
        configureIntTimer();
    }
}
