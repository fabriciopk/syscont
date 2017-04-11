#include "gprs.h"

char *AT_COMMANDS[] = {
    "AT\r\n",
    "ATE0\r\n",
    "AT+IPR=115200\r\n",
    "AT+CREG?\r\n",
    "AT+COPS=0,0\r\n",
    "AT+COPS?\r\n",
    "AT+CGATT=1\r\n",
    "AT+CGDCONT=1, \"IP\", \""APN"\"\r\n",
    "AT+CGACT=1,1\r\n",
    "AT+CIFSR\r\n",
    "AT+CIPSTART=\"TCP\",\""URL"\",80\r\n",
    "AT+CIPSEND=",
    "AT+CIPSHUT\r\n",
    "AT+GPS=1\r\n",
    "AT+GPS=0\r\n",
    "AT+GPSRD=1\r\n",
    "AT+GPSRD=0\r\n"
};

char *AT_ANS[] = {
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "CREG: 1",
    "OK\r\n",
    "COPS: 0",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "> ",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n"
};


void gprs_init(){
    init = 0;
    connected = 0;
    data_sent = 0;
    
    // wait the module initialization proccess
#ifdef TELIT_SIM
    while (waitFor("CREG: 5\r\n", 0, 20000) != 1){
#else
    while (waitFor("CREG: 1\r\n", 0, 20000) != 1){
#endif
        gprs_powerCycle();
        uart_buffer_clear();
    }
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[DIS_ECHO]);
    waitFor(AT_ANS[DIS_ECHO], 0, 2000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[SET_BAUD_RATE]);
    waitFor(AT_ANS[SET_BAUD_RATE], 0, 2000);
    
    do{
        uart_buffer_clear();
        uart_send(AT_COMMANDS[NETWORK_REGIST]);
    } while(waitFor(AT_ANS[NETWORK_REGIST], 0, 5000) != 1);
    uart_buffer_clear();
    
#ifdef DEBUG
    uart_send(AT_COMMANDS[EN_SHOW_OPERATOR]);
    waitFor(AT_ANS[EN_SHOW_OPERATOR], 0, 5000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[CHECK_OPERATOR]);
    waitFor(AT_ANS[CHECK_OPERATOR], 0, 7000);
    uart_buffer_clear();
#endif
}

void gprs_connect(){
    uart_send(AT_COMMANDS[ATTACH]);
    waitFor(AT_ANS[ATTACH], 0, 15000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[SET_PDP_CONTEXT]);
    waitFor(AT_ANS[SET_PDP_CONTEXT], 0, 3000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[ACTIVATE_PDP_CONTEXT]);
    waitFor(AT_ANS[ACTIVATE_PDP_CONTEXT], "ERROR", 10000);
    uart_buffer_clear();
    
#ifdef DEBUG
    uart_send(AT_COMMANDS[GET_IP]);
    waitFor(AT_ANS[GET_IP], 0, 7000);
#endif
    
    do{
        uart_buffer_clear();
        uart_send(AT_COMMANDS[CONN_TCP]);
    } while((init = waitFor(AT_ANS[CONN_TCP], "ERROR", 20000)) != 1);
    uart_buffer_clear();
    
}

/*
void gprs_auth(){
    gprs_connect();
    
    uart_send(AT_COMMANDS[SEND_DATA]);
    uart_send_int(sizeof(API_AUTH) + sizeof(THING_KEY) + sizeof(APP_ID) + 
        sizeof (APP_TOKEN) - 10 + sizeof(POST_HEADER));  // -6 %s -4 \0
    uart_send("\r\n");
    
    waitFor(AT_ANS[SEND_DATA], 0, 5000);
    uart_buffer_clear();
    
    UC0IE &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
    
    sprintf(UART_BUFFER, POST_HEADER, (sizeof(API_AUTH) + sizeof(THING_KEY) + 
        sizeof(APP_ID) + sizeof (APP_TOKEN) - 10));
    uart_send(UART_BUFFER);
    
    sprintf(UART_BUFFER, API_AUTH, APP_TOKEN, APP_ID, THING_KEY);
    uart_send(UART_BUFFER);
    uart_buffer_clear();
    
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    
    if (waitFor("sessionId", "ERROR", 15000) == 1){ // extract sessionId
        unsigned char sessionIdIndex = buffer_find("\"sessionId\":\"");
        
        unsigned char i=0;
        while(UART_BUFFER[sessionIdIndex] != '\"'){
            sessionId[i++] = UART_BUFFER[sessionIdIndex++];
        }
        
        sessionId[i] = '\0';
        connected = 1;
    }
    uart_buffer_clear();
}

void gprs_send_volume(int value){
    uart_send(AT_COMMANDS[SEND_DATA]);
    uart_send_int(sizeof(API_PUBLISH) + sizeof(THING_KEY) + 
        strlen(sessionId) + sizeof(POST_HEADER) -6);  // -8 %s -3 '\0' +2 value +3 content-length
    uart_send("\r\n");
    
    waitFor(AT_ANS[SEND_DATA], 0, 5000);
    uart_buffer_clear();
    
    UC0IE &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
    
    sprintf(UART_BUFFER, POST_HEADER, (sizeof(API_PUBLISH) + sizeof(THING_KEY) + strlen(sessionId) -6));
    uart_send(UART_BUFFER);
    
    sprintf(UART_BUFFER, API_PUBLISH, sessionId, THING_KEY, value);
    uart_send(UART_BUFFER);
    uart_buffer_clear();
    
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    
    if (waitFor("success\":true", "ERROR", 17000) == 1){
        data_sent = 1;
    }
    uart_buffer_clear();
}

void gprs_disconnect(){
    connected = 0;
    
    uart_send(AT_COMMANDS[SEND_DATA]);
    uart_send_int(sizeof(POST_HEADER) + sizeof(API_END_SESSION) + 2*strlen(sessionId) -5);
    uart_send("\r\n");
    
    waitFor(AT_ANS[SEND_DATA], 0, 5000);
    uart_buffer_clear();
    
    UC0IE &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
    
    sprintf(UART_BUFFER, POST_HEADER, (sizeof(API_END_SESSION) + 2*strlen(sessionId) -5));
    uart_send(UART_BUFFER);
    
    sprintf(UART_BUFFER, API_END_SESSION, sessionId, sessionId);
    uart_send(UART_BUFFER);
    uart_buffer_clear();
    
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    waitFor("success\":true", "ERROR", 17000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[CLOSE_TCP]);
    waitFor("OK\r\n", 0, 10000);
    
    UC0IE &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
}
*/

void gprs_send_volume(float value){
    gprs_connect();
    
    uart_send(AT_COMMANDS[SEND_DATA]);
    
    uart_send_int(sizeof(POST_HEADER) + sizeof(POST_API) + 5);
    uart_send("\r\n");
    
    waitFor(AT_ANS[SEND_DATA], 0, 5000);
    uart_buffer_clear();
    
    UC0IE &= ~UCA0RXIE; // Disable USCI_A0 RX interrupt
    
    sprintf(UART_BUFFER, POST_HEADER, sizeof(POST_API) + 6);
    uart_send(UART_BUFFER);
    
    sprintf(UART_BUFFER, POST_API);
    uart_send(UART_BUFFER);
    uart_send_float(value);
    uart_send("}");
    uart_buffer_clear();
    
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    
    if (waitFor("success\": true", "ERROR", 17000) == 1){
        data_sent = 1;
    }
    uart_buffer_clear();
}

void get_coordinates(){
    uart_send(AT_COMMANDS[GPS_ON]);
    waitFor(AT_ANS[GPS_ON], 0, 10000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[GPS_AT_ON]);
    waitFor(AT_ANS[GPS_AT_ON], 0, 5000);
    uart_buffer_clear();
    
    unsigned char i;
    for (i=0; i<3; i++){
#ifdef DEBUG
        ledToggle(RED_LED);
#endif
        waitFor("GPGGA", "ERROR", 2000);
        uart_buffer_clear();
        
        //if (waitFor("GPRMC", "ERROR", 2000) == 1){
        // extract coordinates
        //}
    }
    
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[GPS_AT_OFF]);
    waitFor(AT_ANS[GPS_AT_OFF], 0, 5000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[GPS_OFF]);
    waitFor(AT_ANS[GPS_OFF], 0, 5000);
    uart_buffer_clear();
}

void gprs_reset(){
    P1OUT |= RST;
    delay(20);
    P1OUT &= ~RST;
    delay(500);
}

void gprs_powerCycle(){
    
    gprs_reset();
    
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    uart_buffer_clear();
    
    P1OUT |= PWR;
    delay (2000);
    P1OUT &= ~PWR;
    delay(500);
}
