#include <msp430.h>
#include <stdint.h>
#include "msp-cpu.h"
#include "utils.h"
#include "sensor.h"
#include "gprs.h"

/*
 * main.c
 */

#define TEMPO_SLEEP 2 // 1 seg
// #define TEMPO_SLEEP 10 // 5 seg
// #define TEMPO_SLEEP 20 // 10 seg
// #define TEMPO_SLEEP 40 // 20 seg
// #define TEMPO_SLEEP 120 // 1 min
// #define TEMPO_SLEEP 240 // 2 min
// #define TEMPO_SLEEP 600 // 5 min
// #define TEMPO_SLEEP 1200 // 10 min
// #define TEMPO_SLEEP 3600 // 30 min
// #define TEMPO_SLEEP 7200 // 1 hr
// #define TEMPO_SLEEP 14400 // 2 hrs
// #define TEMPO_SLEEP 28800 // 4 hrs
// #define TEMPO_SLEEP 43200 // 6 hrs
// #define TEMPO_SLEEP 57600 // 8 hrs
// #define TEMPO_SLEEP 86400 // 12 hrs

volatile static uint32_t i;
volatile float distance;
volatile float battery;
volatile uint8_t counter;

void configureIntTimer(void);

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    msp_init();
    // __bic_SR_register(GIE); // disable global interrupt flag

    delay(1000);
    boardOn();
    delay(2000);

    distance = sensor_get();
    battery = read_battery();
    battery += read_battery();
    battery += read_battery();
    battery += read_battery();
    battery = battery/4;

    counter = 0;
    do {
        if (++counter >= 3) break;
        gprs_powerCycle();
    } while (! (gprs_init() && gprs_send_data(distance, battery)));

    boardOff();

    configureIntTimer();
    LPM0; // LOW POWER MODE 0
    //LPM0_EXIT;
    while(1);
}

void configureIntTimer(void){
    i = 0;
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

        distance = sensor_get();
        battery = read_battery();
        battery += read_battery();
        battery += read_battery();
        battery += read_battery();
        battery = battery/4;

        counter = 0;
        do {
            if (++counter >= 3) break;
            gprs_powerCycle();
        } while (! (gprs_init() && gprs_send_data(distance, battery)));

        // gprs_reset();
        boardOff();

        __bic_SR_register(GIE); // disable global interrupt flag
        configureIntTimer();
    }
}
