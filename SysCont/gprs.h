
#ifndef __sysclean_gprs__
#define __sysclean_gprs__

#include <msp430.h>
#include <stdint.h>
#include <stdio.h>
#include "utils.h"
#include "msp-cpu.h"
#include "paho_mqtt_packet/MQTTPacket.h"

#define DEVICE_ID 10

#define APN "virtueyes.vivo.com.br"
// #define APN "internetm2m.air.com" // telit
// #define APN "zap.vivo.com.br"
// #define APN "timbrasil.br"

// #define SERVER "mqtt.solveiot.com.br"
#define SERVER "54.233.104.252"

// MQTT
// topic: "s</DEVICE_ID>", client_id: "s<DEVICE_ID>"
#define MQTT_TOPIC_PREFIX "s/"
#define MQTT_CLIENT_PREFIX "s"
#define MQTT_USERNAME "dalmago"
#define MQTT_PASSWD "teste123"
#define MQTT_KEEP_ALIVE 20

uint8_t gprs_init();
uint8_t gprs_connect();
void get_coordinates();
void gprs_reset();
void gprs_powerCycle();
uint8_t gprs_send_data(float, float);

// unsigned char init;
// unsigned char connected;
// unsigned char data_sent;

#endif
