// 

#include "apple_23.h"

#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial softserial; //RX, TX
void debug_init();
void debug_msg(char message[]);
void debug_float(float message);
void debug_int(int message);
void debug_double(double message);
#endif

