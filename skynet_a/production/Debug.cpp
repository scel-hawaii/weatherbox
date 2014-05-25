/**************************************************************
 *
 * Debug functions to make turning debugging on and off easier
 *
 *************************************************************/
#include "Debug.h"

SoftwareSerial softserial(4, 5); //RX, TX
schema_6 debug_text;

void sendDebugPacket(char *dtext){
    // payload used for PacketBINARY transmission
    uint8_t rf_payload[243];
    debug_text.schema = 6;
    strcpy(debug_text.text, dtext);

    //
    // TODO: Finish adding debug transmission.
    //

    /*
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &debug_text, sizeof(debug_text));
    ZBTxRequest zbtx = ZBTxRequest(addr64, rf_payload, sizeof(debug_text));
    xbee.send(zbtx);
    */
}


void debug_init(void){
    #ifdef DEBUG_SOFT
    softserial.begin(9600);
    #endif
}
void debug_msg(char message[]){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_float(float message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_int(int message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_double(double message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}
