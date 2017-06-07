
#ifndef __sysclean_utils__
#define __sysclean_utils__

#include <msp430.h>
#include <stdio.h>
#include <string.h>

#define DEBUG

#define RED_LED BIT0
#define GREEN_LED BIT6
#define UART_TXD BIT2
#define UART_RXD BIT1
#define PWR BIT4
#define RST BIT5
#define TRIGGER BIT7
#define ECHO BIT3

#define ledOn(led) (P1OUT |= (led))
#define ledOff(led) (P1OUT &= ~(led))
#define ledToggle(led) (P1OUT ^= (led))

#define BUFFER_SIZE 256
volatile char UART_BUFFER[BUFFER_SIZE];
volatile unsigned int UART_INDEX;
volatile unsigned short TIMER_WAITFOR_COUNTER;

void msp_init();
void uart_send(const char*);
void uart_send_buffer(const char*, int);
void uart_send_int(int);
void uart_buffer_clear();
void delay(unsigned short);
int buffer_find(const char *);
int waitFor(const char*, const char*, unsigned short);
void floatToBytes(float, unsigned char*, unsigned char*);
void generatePayload(unsigned char*, float, float);

#endif
