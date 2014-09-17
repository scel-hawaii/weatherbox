// Arduino Xbee Library RX Test

#include <XBee.h>

XBee xbee = XBee();
SoftwareSerial softserial(12, 11); //RX, TX
ZBRxResponse rx = ZBRxResponse();

// XBeeAddress64 addr64 = XBeeAddress64(0, 0);

void setup(){
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);
}

void loop(){
    // Attempt to read an API packet from the ZBee Module
    xbee.readPacket();

    if (xbee.getResponse().isAvailable()) {
        Serial.println("Okay something is available");
        if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {
            // now fill our zb rx class
            xbee.getResponse().getZBRxResponse(rx);

            Serial.println("Got an rx packet!");

            if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
                // the sender got an ACK
                // flashLed(statusLed, 10, 100);
                Serial.println("packet acknowledged");
            } 
            else {
                // we got it (obviously) but sender didn't get an ACK
                // flashLed(errorLed, 2, 100);
                Serial.println("packet not acknowledged");
            }

            data
            Serial.print("Here is the data we got: ");
            Serial.pritnln("
        }

    }
}
