#include <Arduino.h>

#include "driver.h"
#include "dac.h"
#include "cmd_parser.h"

void main_routine(){
    // Run this forever
    TesterCommand user_command;


    while(1){
        CMD_fetch(&user_command);
        print_cmd(&user_command);

        Serial.println(user_command.token);
        DAC_initDac();
        DAC_setVoltage(3.3);
    }

}


void print_cmd(TesterCommand* user_command){
    int i = 0;
    while( (user_command->cmd[i]) != 0 ){
        Serial.print(user_command->cmd[i]);
    }
}
