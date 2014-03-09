#ifndef DRIVER_H
#define DRIVER_H

#include "uart.h"
#include "dac.h"
#include "cmd_parser.h"

void main_routine(void);
void print_cmd(TesterCommand *user_command);

#endif
