#include "uart.h"
#define MAX_CMD_LEN 10
#define MAX_ARG_LEN 10

typedef struct tester_command{ 
    char cmd[MAX_CMD_LEN];
    char token[MAX_ARG_LEN];
} TesterCommand;
