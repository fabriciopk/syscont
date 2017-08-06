#include "gprs.h"

/*
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
*/

const char MQTT_CONNECT[] = {
                              0x10,  // CONNECT
                              35,     // Remaining Length
                              0, 6, 'M', 'Q', 'I', 's', 'd', 'p', 3, // Protocol
                              0xC2,   // Username Flag + Password Flag + Clean Session
                              0, 20,  // Keep Alive
                              0, 2, 'c', DEVICE_ID, // Client Id
                              0, 7, 'd', 'a', 'l', 'm', 'a', 'g', 'o', // Username
                              0, 8, 't', 'e', 's', 't', 'e', '1', '2', '3' // Password
                            };

void gprs_init(){
    init = 0;
    connected = 0;
    data_sent = 0;
    
    // wait the module initialization proccess
    // CREG: 1 - local network
    // CREG: 5 - roaming
    while (! waitFor("CREG: 1\r\n", "CREG: 5\r\n", 25000)){
        gprs_powerCycle();
        uart_buffer_clear();
    }
    uart_buffer_clear();
    
    uart_send("ATE0\r\n"); // DISABLE ECHO
    waitFor("OK\r\n", 0, 2000);
    uart_buffer_clear();
    
    uart_send("AT+IPR=115200\r\n"); // SET BAUD RATE
    waitFor("OK\r\n", 0, 2000);
    
    do{
        uart_buffer_clear();
        uart_send("AT+CREG?\r\n"); // NETWORK REGISTER
    } while(waitFor("CREG: 1", 0, 5000) != 1);
    uart_buffer_clear();
    
#ifdef DEBUG
    uart_send("AT+COPS=3,0\r\n"); // ENABLE SHOW OPERATOR
    waitFor("OK\r\n", 0, 2000);
    uart_buffer_clear();
    
    uart_send("AT+COPS?\r\n"); // CHECK OPERATOR
    waitFor("COPS: 1,", 0, 2000);
    uart_buffer_clear();
#endif
}

void gprs_connect(){
    uint8_t counter;
    
    while(1){
        uart_buffer_clear();
        uart_send("AT+CGATT=0\r\n"); // DETACH
        waitFor("OK\r\n", 0, 25000);
        
        counter = 0;
        do{
            if (counter++ >= 3) break;
            uart_buffer_clear();
            uart_send("AT+CGATT=1\r\n"); // ATTACH
        } while (waitFor("OK\r\n", "ERROR", 25000) != 1);
        
        if (counter >= 3) continue;
        
        uart_buffer_clear();
        uart_send("AT+CGDCONT=1,\"IP\",\""APN"\"\r\n"); // SET PDP CONTEXT
        waitFor("OK\r\n", 0, 3000);
        
        counter = 0;
        do{
            if (counter++ >= 3) break;
            uart_buffer_clear();
            uart_send("AT+CGACT=1,1\r\n"); // ACTIVATE PDP
        } while(waitFor("OK\r\n", "ERROR", 35000) != 1);
        
        if (counter >= 3) continue;
        break;
    }
    
#ifdef DEBUG
    uart_buffer_clear();
    uart_send("AT+CIFSR\r\n"); // READ IP
    waitFor("OK\r\n", 0, 7000);
#endif
    
    counter = 0;
    do{
        if (counter++ >= 5){
            uart_buffer_clear();
            uart_send("AT+CIPSHUT\r\n"); // CLOSE TCP
            waitFor("OK\r\n", "ERROR", 5000);
            counter = 0;
        }
        uart_buffer_clear();
        uart_send("AT+CIPSTART=\"TCP\",\""URL"\",1883\r\n"); // CON TCP
    } while((init = waitFor("OK\r\n", "ERROR", 25000)) != 1);
    uart_buffer_clear();
    
    return;
}


void gprs_send_data(float distance1, float distance2, float battery){
    gprs_connect();
    
    unsigned char payload[9];
    generatePayload(payload, distance1, distance2, battery);
    
    // MQTT
    unsigned char publish[4];
    publish[0] = MQTT_PUBLISH_FIRST_BYTE;
    publish[1] = sizeof(payload) + sizeof(MQTT_PUBLISH_TOPIC) - 1 + 2;
    publish[2] = 0;
    publish[3] = sizeof(MQTT_PUBLISH_TOPIC) -1;
    
    uart_buffer_clear();
    uart_send("AT+CIPSEND=");
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
