#include "gprs.h"

uint8_t gprs_init(){
    // init = 0;
    // connected = 0;
    // data_sent = 0;
    uint8_t counter = 0;

    uart_buffer_clear();

    // wait the module initialization proccess
    // CREG: 1 - local network
    // CREG: 5 - roaming
    while (! waitFor("CREG: 1\r\n", "CREG: 5\r\n", 40000)){
        if (++counter >= 5) return 0;
        gprs_powerCycle();
        uart_buffer_clear();
    }
    uart_buffer_clear();

    uart_send("ATE0\r\n");  // DIS_ECHO
    waitFor("OK\r\n", 0, 2000);
    uart_buffer_clear();

    uart_send("AT+IPR=115200\r\n"); // SET_BAUD_RATE
    waitFor("OK\r\n", 0, 2000);

    counter = 0;
    do{
        if (++counter >= 3) return 0;
        uart_buffer_clear();
        uart_send("AT+CREG?\r\n"); // NETWORK_REGIST
    } while(waitFor("CREG: 1", 0, 5000) != 1);
    uart_buffer_clear();

#ifdef DEBUG
    // uart_send("AT+COPS=1,2,\"72404\"\r\n"); // SET_OPERATOR
    // waitFor("OK\r\n", 0, 20000);
    // uart_buffer_clear();

    uart_send("AT+COPS=3,0\r\n"); // EN_SHOW_OPERATOR
    waitFor("OK\r\n", 0, 2000);
    uart_buffer_clear();

    uart_send("AT+COPS?\r\n"); // CHECK_OPERATOR
    waitFor("OK\r\n", 0, 2000);
    uart_buffer_clear();
#endif

    return 1;
}

uint8_t gprs_connect(){
    uint8_t counter1, counter2;

    while(1){
        counter1 = 0;
        counter2 = 0;

        do{
            if (counter1++ >= 3) return 0;

            uart_buffer_clear();
            uart_send("AT+CGATT=0\r\n"); // DETACH
            waitFor("OK\r\n", 0, 25000);

            uart_buffer_clear();
            uart_send("AT+CGATT=1\r\n");  // ATTATCH
        } while (waitFor("OK\r\n", "ERROR", 25000) != 1);

        // if (counter >= 3) continue;

        uart_buffer_clear();
        uart_send("AT+CGDCONT=1,\"IP\",\""APN"\"\r\n"); // SET_PDP_CONTEXT
        waitFor("OK\r\n", 0, 3000);

        counter1 = 0;
        do{
            if (counter1++ >= 3) break;
            uart_buffer_clear();
            uart_send("AT+CGACT=1,1\r\n"); // ACTIVATE_PDP_CONTEXT
        } while(waitFor("OK\r\n", "ERROR", 35000) != 1);

        if (counter2++ >= 3) return 0;
        if (counter1 >= 3) continue;
        break;
    }

#ifdef DEBUG
    uart_buffer_clear();
    uart_send("AT+CIFSR\r\n"); // GET_IP
    waitFor("OK\r\n", 0, 7000);
#endif

    counter1 = 0;
    counter2 = 0;
    do{
        if (++counter1 >= 3){
            if (++counter2 >= 3) return 0;

            uart_buffer_clear();
            uart_send("AT+CIPSHUT\r\n"); // CLOSE TCP
            waitFor("OK\r\n", "ERROR", 5000);
            counter1 = 0;
        }
        uart_buffer_clear();
        uart_send("AT+CIPSTART=\"TCP\",\""SERVER"\",1883\r\n"); // CONN_TCP
    } while(waitFor("OK\r\n", "ERROR", 15000) != 1);
    uart_buffer_clear();

    return 1;
}


uint8_t gprs_send_data(float distance, float battery){
    if (! gprs_connect()) return 0;
    uint8_t len;

    uint8_t payload[5];
    generatePayload(payload, distance, battery);

    uint8_t buf[100];
    uint8_t dev_id[6];
    uitoa(dev_id, DEVICE_ID);

    uint8_t topic[sizeof(MQTT_TOPIC_PREFIX) + 5] = {MQTT_TOPIC_PREFIX};
    strcat(topic, dev_id);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    MQTTString topicString = MQTTString_initializer;

    data.clientID.cstring = "";
    data.keepAliveInterval = MQTT_KEEP_ALIVE;
    data.cleansession = 1;
    data.username.cstring = MQTT_USERNAME;
    data.password.cstring = MQTT_PASSWD;
    data.MQTTVersion = 3;
    topicString.cstring = topic;

    len = MQTTSerialize_connect(buf, sizeof(buf), &data);
    len += MQTTSerialize_publish((buf + len), sizeof(buf) - len, 0, 0, 0, 0, topicString, payload, sizeof(payload));
    len += MQTTSerialize_disconnect((buf + len), sizeof(buf) - len);

    uart_buffer_clear();

    uart_send("AT+CIPSEND="); // SEND DATA
    uart_send_int(len);
    uart_send("\r\n");

    waitFor("> ", 0, 3500);
    uart_buffer_clear();

    uart_send_buffer(buf, len);

    uart_buffer_clear();
    return waitFor("CIPRCV", 0, 10000);

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
