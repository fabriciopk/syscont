
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

#define APP_TOKEN "r2787aOAqE4ltiKG"
#define APP_ID "teste123"
#define THING_KEY "msp430"

#define TOKEN_API "5GYS1fPfJKOLG2h13Hzc"
#define DEVICE_ID "2"
#define URL "api.solveiot.com.br"

#define POST_HEADER "POST /api HTTP/1.1\r\nHost: api.solveiot.com.br\r\nContent-Length: %d\r\n\r\n" // 67 + 3 = 70
#define POST_API "{\"autent\":\""TOKEN_API"\",\"id\":"DEVICE_ID",\"waste\":" // 27 + 20 (token) + 1 (id) // + 6 (waste)

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
