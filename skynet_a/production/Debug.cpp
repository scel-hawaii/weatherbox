/**************************************************************
 *
 * Debug functions to make turning debugging on and off easier
 *
 *************************************************************/

#include "Debug.h"
SoftwareSerial softserial(4, 5); //RX, TX

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
