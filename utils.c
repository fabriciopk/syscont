#include "utils.h"

void msp_init(){
    
    DCOCTL = 0; // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ; // Set DCO
    DCOCTL = CALDCO_1MHZ;
 
 #ifdef DEBUG   
    P1DIR |= RED_LED + GREEN_LED + RST + PWR + TRIGGER; // outputs
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
    int i;
    
    for (i=0; i<len; i++){
        UCA0TXBUF = buf[i];
        while(! (UC0IFG & UCA0TXIFG));
    }
    
}

void uart_send_int(int value){
    char string[5];
    
    sprintf(string, "%d", value);
    uart_send(string);
}

void uart_send_float(float value){ // %6.2f

    char string[7];
    int integer;
    int decimal;
    
    string[3] = '.';
    string[6] = '\0';
    
    integer = (int)value;
    decimal = (int)((value - integer) * 100);
    
    string[5] = (decimal % 10) + '0';
    decimal = decimal / 10;
    string[4] = (decimal % 10) + '0';
    
    string[2] = (integer % 10) + '0';
    integer = integer / 10;
    
    string[1] = (integer > 0)? ((integer % 10) + '0') : ' ';
    integer = integer / 10;
    
    string[0] = (integer > 0)? ((integer % 10) + '0') : ' ';
    
    // // if (integer >= 100)
    // string[0] = (integer >= 100)? ((integer/100)+'0') : ' ';
    
    
    
    // if (integer >= 100)
    //     sprintf(string, "%d.%d", integer, decimal);
    // else
    //     sprintf(string, " %d.%d", integer, decimal);
    
    uart_send(string);

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
    memset(UART_BUFFER, 0, BUFFER_SIZE);
    UART_INDEX = 0;
}

#pragma vector=USCIAB0RX_VECTOR 
__interrupt void USCI0RX_ISR(void) 
{
    UART_BUFFER[UART_INDEX++] = UCA0RXBUF;
    UART_BUFFER[UART_INDEX] = '\0';
}

int buffer_find(const char *string) {
  unsigned int i, j, k;

  for (i=0; i<UART_INDEX; i++) {
    j = i;
    k = 0;
    while (1){
        if (UART_BUFFER[j++] != string[k++]) break;
        if (string[k] == '\0') return j;
    }
  }
  return 0;
}

int waitFor(const char *ans, const char *error, unsigned short time){
    unsigned char counter = 0;
    
    do{
        delay(300);
        
        if (counter++ > (time / 300)){
#ifdef DEBUG
            ledOn(RED_LED);
#endif
            return 0;
        }
        
        if (error != 0){
            if (buffer_find(error)){
#ifdef DEBUG
                ledOn(RED_LED);
#endif
                return -1;
            }
        }
    } while(! buffer_find(ans));
    
#ifdef DEBUG
    ledOff(RED_LED);
#endif
    return 1;
}
