#include <msp430.h> 
#include "utils.h"
#include "sensor.h"
#include "gprs.h"


/*
 * main.c
 */


int main(void)
{
    WDTCTL = WDTPW + WDTHOLD; // Stop WDT
    
    msp_init();
    delay(2000);

    // read sensors
    /*
    gprs_powerCycle();
    gprs_init();
    //get_coordinates();
    gprs_auth();
    gprs_send_volume(56);
    gprs_disconnect();
    gprs_reset();
    */
    
    
    
    while (1)
    {
#ifdef DEBUG
        ledToggle(GREEN_LED);
        delay(500);
#endif

    }
}
