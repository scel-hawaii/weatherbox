/**************************************************************
 *
 * Debug functions to make turning debugging on and off easier
 *
 *************************************************************/

#include "Debug.h"

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
