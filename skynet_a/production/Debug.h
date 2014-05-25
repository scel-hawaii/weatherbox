// 

#include <XBee.h>
#include "apple_23.h"
#include "schema.h"

#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>
#include <SoftwareSerial.h>

extern SoftwareSerial softserial; //RX, TX
extern schema_6 debug_text;

void debug_sendXbeePacket(char *dtext);

void debug_init();
void debug_msg(char message[]);
void debug_float(float message);
void debug_int(int message);
void debug_double(double message);
#endif

