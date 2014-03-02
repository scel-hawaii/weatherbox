#include "cmd_parser.h"
// Pass this function a command char array of length 10
// Function returns a 1 if there are no errors, and 
// a 0 if there are errors 
int CMD_fetch(TesterCommand * full_command){
    char incomingByte[MAX_CMD_LEN];
    char incomingChar = 0;
    int i = 0; 

    // Init the incomingByte array
    for( i = 0; i < MAX_CMD_LEN ; i++ ) {
        incomingByte[i] = 0;
    }

    i = 0; 
    // Poll and check for spaces
    incomingChar = UART_poll();
    while(incomingChar != ' '){
        // If there is a newline here return a zero!
        if(incomingChar == '\n')
            return 0;
        incomingByte[i] = incomingChar;
        incomingChar = UART_poll();
    }

    strcpy(incomingByte, full_command->cmd);

    // After that, parse for the token
    i = 0;
    incomingChar = UART_poll();
    while(incomingChar != '\n'){
        // If there is a space here return a zero!
        if(incomingChar == ' ')
            return 0;
        incomingByte[i] = incomingChar;
        incomingChar = UART_poll();
    }

    // Init the incomingByte array
    for( i = 0; i < MAX_CMD_LEN ; i++ ) {
        incomingByte[i] = 0;
    }

    strcpy(incomingByte, full_command->token);

    return 1;
}
