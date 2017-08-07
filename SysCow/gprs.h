
#ifndef __sysclean_gprs__
#define __sysclean_gprs__

#include <msp430.h>
#include "utils.h"
#include "msp-cpu.h"

// #define APN "virtueyes.vivo.com.br"
#define APN "internetm2m.air.com"
// #define APN "zap.vivo.com.br"

#define DEVICE_ID '3'
// #define URL "mqtt.solveiot.com.br"
#define URL "35.185.210.142"

#define MQTT_PUBLISH_FIRST_BYTE 0x30 // QOS 0

#define MQTT_PUBLISH_TOPIC "c/3"

uint8_t gprs_init();
uint8_t gprs_connect();
void get_coordinates();
void gprs_reset();
void gprs_powerCycle();
uint8_t gprs_send_data(float, float, float);

// unsigned char init;
// unsigned char connected;
// unsigned char data_sent;

#endif
