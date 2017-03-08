
#ifndef __sysclean_gprs__
#define __sysclean_gprs__

#include <msp430.h>
#include "utils.h"

#define TELIT_SIM

#ifdef TELIT_SIM
#define APN "internetm2m.air.com"
#else
#define APN "zap.vivo.com.br"
#endif


#define APP_TOKEN "r2787aOAqE4ltiKG"
#define APP_ID "teste123"
#define THING_KEY "msp430"

#define POST_HEADER "POST /api HTTP/1.1\r\nHost: api.devicewise.com\r\nContent-Length: %d\r\n\r\n" // 66 + 3 = 69
#define API_AUTH "{\"auth\":{\"command\":\"api.authenticate\",\"params\":{\"appToken\":\"%s\",\"appId\":\"%s\",\"thingKey\":\"%s\"}}}" // 89 + 30 = 119
#define API_PUBLISH "{\"auth\":{\"sessionId\":\"%s\"},\"1\":{\"command\":\"property.publish\",\"params\":{\"thingKey\":\"%s\",\"key\":\"trash_level\",\"value\":%d}}}" // 114
#define API_END_SESSION "{\"auth\":{\"sessionId\":\"%s\"},\"1\":{\"command\":\"session.end\",\"params\":{\"id\":\"%s\"}}}"

void gprs_init();
void gprs_connect();
void gprs_auth();
void gprs_send_volume(int);
void gprs_disconnect();
void get_coordinates();
void gprs_reset();
void gprs_powerCycle();

unsigned char init;
unsigned char connected;
unsigned char data_sent;
char sessionId[30];

enum at_command {
    AT = 0,
    DIS_ECHO,
    SET_BAUD_RATE,
    NETWORK_REGIST,
    EN_SHOW_OPERATOR,
    CHECK_OPERATOR,
    ATTACH,
    SET_PDP_CONTEXT,
    ACTIVATE_PDP_CONTEXT,
    GET_IP,
    CONN_TCP,
    SEND_DATA,
    CLOSE_TCP,
    GPS_ON,
    GPS_OFF,
    GPS_AT_ON,
    GPS_AT_OFF,
};

#endif
