/****************************
 *
 *
 *
 *
 *****************************/



#include <SoftwareSerial.h>
#include <XBee.h>
#include <Arduino.h>
#include "schema.h"

SoftwareSerial softserial(2, 3);
XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);
schema_3 packet;

void setup(){
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);

    // Hardcode these values at the beginning so we 
    // can test this without having to worry
    packet.schema = 3;
    packet.address = 10;
    packet.uptime_ms = millis();
    packet.n = 0;

}

void loop(){
    while(1){
        send_data(packet);
        delay(1000);
        packet.uptime_ms = millis();
        packet.n++;
    }
}


void send_data(schema_3 packet){
    uint8_t rf_payload[83];
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
}


