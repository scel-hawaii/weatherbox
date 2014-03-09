#include "setup.h"
void setup_ports(){

}

void setup_uart(){
    UART_init();
}

void setup_routine(){
    setup_ports();
    setup_uart();
}
