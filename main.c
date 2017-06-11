#include <msp430.h>
#include "msp-cpu.h"
#include "utils.h"
#include "sensor.h"
#include "gprs.h"

/*
 * main.c
 */
 
// #define TEMPO_SLEEP 40 // 20 seg
#define TEMPO_SLEEP 120 // 1 min
// #define TEMPO_SLEEP 240 // 2 min
// #define TEMPO_SLEEP 600 // 5 min
// #define TEMPO_SLEEP 28800 // 4 hrs
 
volatile static unsigned int i;
//volatile static unsigned int tempo; // * 0.5s
volatile float distance;
volatile float battery;

 
void configureIntTimer(void);
 
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    i = 0;
    msp_init();
    read_battery();
    // __bic_SR_register(GIE); // disable global interrupt flag
    
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
        
        distance = sensor_get();
        battery = read_battery();
        
        gprs_powerCycle();
        gprs_init();
        gprs_send_data(distance, battery);
        
        // gprs_reset();
        boardOff();
        
        i = 0;
        __bic_SR_register(GIE); // disable global interrupt flag
        configureIntTimer();
    }
}
