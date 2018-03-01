#ifndef __sysclean_msp_cpu__
#define __sysclean_msp_cpu__

#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "utils.h"

#define ledOn(led) (P1OUT |= (led))
#define ledOff(led) (P1OUT &= ~(led))
#define ledToggle(led) (P1OUT ^= (led))

void msp_init();
void uart_send(const char*);
void uart_send_buffer(const char*, int);
void delay(unsigned short);
void uart_buffer_clear();
float read_battery();
void boardOn();
void boardOff();

#endif