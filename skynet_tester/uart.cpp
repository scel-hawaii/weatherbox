#include "uart.h"

void UART_init(void){
    Serial.begin(9600);
}

char UART_poll(void){
    if(Serial.available() > 0){
        return Serial.read();
    }
}
