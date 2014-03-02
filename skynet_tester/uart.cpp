#include "uart.h"

#ifndef UART_H
#define UART_H
void UART_init(void){
    Serial.begin(9600);
}

char UART_poll(void){
    if(Serial.available() > 0){
        return Serial.read();
    }
}

#endif
