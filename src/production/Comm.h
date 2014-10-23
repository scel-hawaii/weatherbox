// Comm.h
#ifndef COMM_H
#define COMM_H

#include <stdint.h>
#include <XBee.h>
#include <Arduino.h>
#include "Debug.h"

extern XBee xbee;
extern XBeeAddress64 addr64;

void Comm_transmitPacketXbee(uint8_t rf_payload);
void Comm_initXbee();

#endif
