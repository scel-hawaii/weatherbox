// PacketBINARY.h

#ifndef PACKETBINARY_H
#define PACKETBINARY_H

#include <Arduino.h>
#include "Sensors.h"
#include "overflow_checker.h"
#include "schema.h"
#include "apple_23.h"

schema_3 packet;

void Packet_init(void);
void Packet_samplePacketBinary(void);
void Packet_transmitPacketBinary(void);

#endif
