#include "gprs.h"

char *AT_COMMANDS[] = {
    "AT\r\n",
    "ATE0\r\n",
    "AT+IPR=115200\r\n",
    "AT+CREG?\r\n",
    "AT+COPS=3,0\r\n",
    "AT+COPS?\r\n",
    "AT+CGATT=1\r\n",
    "AT+CGDCONT=1,\"IP\",\""APN"\"\r\n",
    "AT+CGACT=1,1\r\n",
    "AT+CIFSR\r\n",
    "AT+CIPSTART=\"TCP\",\""URL"\",1883\r\n",
    "AT+CIPSEND=",
    "AT+CIPSHUT\r\n"
};

char *AT_ANS[] = {
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "CREG: 1",
    "OK\r\n",
    "COPS: 1,",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n",
    "OK\r\n"
};

const char MQTT_CONNECT[] = {
                              0x10,  // CONNECT
                              35,     // Remaining Length
                              0, 6, 'M', 'Q', 'I', 's', 'd', 'p', 3, // Protocol
                              0xC2,   // Username Flag + Password Flag + Clean Session
                              0, 20,  // Keep Alive
                              0, 2, 's', DEVICE_ID, // Client Id
                              0, 7, 'd', 'a', 'l', 'm', 'a', 'g', 'o', // Username
                              0, 8, 't', 'e', 's', 't', 'e', '1', '2', '3' // Password
                            };

void gprs_init(){
    init = 0;
    connected = 0;
    data_sent = 0;
    
    // wait the module initialization proccess
#ifdef TELIT_SIM
    while (waitFor("CREG: 5\r\n", 0, 25000) != 1){
#else
    while (waitFor("CREG: 1\r\n", 0, 25000) != 1){
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
    waitFor(AT_ANS[EN_SHOW_OPERATOR], 0, 2000);
    uart_buffer_clear();
    
    uart_send(AT_COMMANDS[CHECK_OPERATOR]);
    waitFor(AT_ANS[CHECK_OPERATOR], 0, 2000);
    uart_buffer_clear();
#endif
}

void gprs_connect(){
    uint8_t counter;
    
    while(1){
        uart_buffer_clear();
        uart_send("AT+CGATT=0\r\n");
        waitFor("OK\r\n", 0, 25000);
        
        counter = 0;
        do{
            if (counter++ >= 3) break;
            uart_buffer_clear();
            uart_send(AT_COMMANDS[ATTACH]);
        } while (waitFor(AT_ANS[ATTACH], "ERROR", 25000) != 1);
        
        if (counter >= 3) continue;
        
        uart_buffer_clear();
        uart_send(AT_COMMANDS[SET_PDP_CONTEXT]);
        waitFor(AT_ANS[SET_PDP_CONTEXT], 0, 3000);
        
        counter = 0;
        do{
            if (counter++ >= 3) break;
            uart_buffer_clear();
            uart_send(AT_COMMANDS[ACTIVATE_PDP_CONTEXT]);
        } while(waitFor(AT_ANS[ACTIVATE_PDP_CONTEXT], "ERROR", 35000) != 1);
        
        if (counter >= 3) continue;
        break;
    }
    
#ifdef DEBUG
    uart_buffer_clear();
    uart_send(AT_COMMANDS[GET_IP]);
    waitFor(AT_ANS[GET_IP], 0, 7000);
#endif
    
    counter = 0;
    do{
        if (counter++ >= 5){
            uart_buffer_clear();
            uart_send(AT_COMMANDS[CLOSE_TCP]);
            waitFor(AT_ANS[CLOSE_TCP], "ERROR", 5000);
            counter = 0;
        }
        uart_buffer_clear();
        uart_send(AT_COMMANDS[CONN_TCP]);
    } while((init = waitFor(AT_ANS[CONN_TCP], "ERROR", 25000)) != 1);
    uart_buffer_clear();
    
    return;
}


#ifdef TWO_SENSORS
void gprs_send_data(float distance, float distance2, float battery){
#else
void gprs_send_data(float distance, float battery){
#endif
    gprs_connect();
    
#ifdef TWO_SENSORS
    unsigned char payload[7];
    generatePayload2(payload, distance, distance2, battery);
#else
    unsigned char payload[5];
    generatePayload(payload, distance, battery);
#endif
    
    // MQTT
    unsigned char publish[4];
    publish[0] = MQTT_PUBLISH_FIRST_BYTE;
    publish[1] = sizeof(payload) + sizeof(MQTT_PUBLISH_TOPIC) - 1 + 2;
    publish[2] = 0;
    publish[3] = sizeof(MQTT_PUBLISH_TOPIC) -1;
    
    uart_buffer_clear();
    uart_send(AT_COMMANDS[11]);
    uart_send_int(sizeof(MQTT_CONNECT) + sizeof(publish) + 
                            sizeof(MQTT_PUBLISH_TOPIC) - 1 + sizeof(payload));
    uart_send("\r\n");
    
    waitFor("> ", 0, 3500);
    uart_buffer_clear();
    
    uart_send_buffer(MQTT_CONNECT, sizeof(MQTT_CONNECT));
    uart_send_buffer(publish, sizeof(publish));
    uart_send(MQTT_PUBLISH_TOPIC);
    uart_send_buffer(payload, sizeof(payload));
    
    uart_buffer_clear();
    waitFor("CIPRCV", 0, 10000);
    
    uart_buffer_clear();
    
    // data_sent?
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
