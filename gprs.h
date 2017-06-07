
#ifndef __sysclean_gprs__
#define __sysclean_gprs__

#include <msp430.h>
#include "utils.h"

// #define TELIT_SIM

#ifdef TELIT_SIM
#define APN "internetm2m.air.com"
#else
#define APN "zap.vivo.com.br"
#endif

#define TOKEN_API ""
#define DEVICE_ID '2'
#define URL "mqtt.dalmago.xyz"
                                
#define MQTT_PUBLISH_FIRST_BYTE 0x30 // QOS 0
#define MQTT_PUBLISH_TOPIC "s/d"

void gprs_init();
void gprs_connect();
void gprs_send_volume(float);
void get_coordinates();
void gprs_reset();
void gprs_powerCycle();

unsigned char init;
unsigned char connected;
unsigned char data_sent;
char sessionId[30];

enum at_command {
    AT = 0,
    DIS_ECHO = 1,
    SET_BAUD_RATE = 2,
    NETWORK_REGIST = 3,
    EN_SHOW_OPERATOR = 4,
    CHECK_OPERATOR = 5,
    ATTACH = 6,
    SET_PDP_CONTEXT = 7,
    ACTIVATE_PDP_CONTEXT = 8,
    GET_IP = 9,
    CONN_TCP = 10,
    SEND_DATA = 11,
    CLOSE_TCP = 12
};

#endif
