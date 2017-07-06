
#ifndef __sysclean_utils__
#define __sysclean_utils__

#include <msp430.h>
#include "msp-cpu.h"

// #define DEBUG

// P1
#define RED_LED BIT0
#define UART_RXD BIT1
#define UART_TXD BIT2
#define ECHO BIT3
#define PWR BIT4
#define RST BIT5
#define VCC_2 BIT6
#define TRIGGER BIT7

// P2
#define BOARD_ON_OFF BIT0

#define VOLTAGE_DIVIDER_FACTOR 4 // for the VCC

#define BUFFER_SIZE 256
volatile char UART_BUFFER[BUFFER_SIZE];
volatile unsigned int UART_INDEX;
// volatile unsigned short TIMER_WAITFOR_COUNTER;

void uart_send_int(int);
int buffer_find(const char *);
int waitFor(const char*, const char*, unsigned short);
void floatToBytes(float, unsigned char*, unsigned char*);
void generatePayload(unsigned char*, float, float);

#endif
