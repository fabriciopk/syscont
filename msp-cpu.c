#include "msp-cpu.h"

void msp_init(){
    
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ; // Set DCO
    DCOCTL = CALDCO_1MHZ;
 
 #ifdef DEBUG   
    P1DIR |= RED_LED + RST + PWR + TRIGGER; // outputs
#else
    P1DIR |= RST + PWR + TRIGGER; // outputs
#endif
    P1DIR &= ~ECHO; // input
    P1OUT = 0;
    P1SEL |= UART_RXD + UART_TXD ; // P1.1 = RXD, P1.2=TXD
    P1SEL2 |= UART_RXD + UART_TXD ;
    
    // reduce power consumption
    P2DIR = 0xFF; // All P2.x outputs
    P2OUT = 0; // All P2.x reset
    
    UCA0CTL1 |= UCSSEL_2; // SMCLK
    
    UCA0BR0 = 0x08; // 1MHz 115200
    UCA0BR1 = 0x00; // 1MHz 115200

    UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5
    
    UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
    
    __bis_SR_register(GIE); // enable global interrupt flag
}

void uart_send(const char* string){
    unsigned int i = 0;

    while (string[i] != '\0'){
        UCA0TXBUF = string[i++];
        while(! (UC0IFG & UCA0TXIFG));
    }
}

void uart_send_buffer(const char *buf, int len){
    unsigned int i;
    
    for (i=0; i<len; i++){
        UCA0TXBUF = buf[i];
        while(! (UC0IFG & UCA0TXIFG));
    }
    
}

void delay(unsigned short value){
    int i;
    
    TACTL = TACLR;
    TACCR0 = 125; // 1ms
    for (i=value; i > 0; i--){
        TACTL = TASSEL_2 + MC_1 + ID_3; // SMCLK/8, upmode
        
        while (! (TACTL & TAIFG));
        
        TACTL = TACLR;
    }
}

void uart_buffer_clear(){
    memset((void*)UART_BUFFER, 0, BUFFER_SIZE);
    UART_INDEX = 0;
}

#pragma vector=USCIAB0RX_VECTOR 
__interrupt void USCI0RX_ISR(void) 
{
    UART_BUFFER[UART_INDEX++] = UCA0RXBUF;
    UART_BUFFER[UART_INDEX] = '\0';
}
