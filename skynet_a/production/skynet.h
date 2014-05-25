/****************************************************
 *
 *          skynet_b03.h
 *          REIS Weatherbox Firmware b03
 *          
 *          The main driver header file for the REIS weatherbox. 
 *
 ****************************************************/


#ifndef SKYNET_H
#define SKYNET_H
#include "LowPass.h"
#include "overflow_checker.h"

/* Can't move libraries into skynet_b03.h. For some reason the program will 
   not compile if that happens. */

// explain what libraries are for and where to find them
#include <SoftwareSerial.h>
#include <XBee.h>

#include <EEPROM.h>

#include "schema.h"
#include "Debug.h"
#include "Comm.h"
#include "PacketHEALTH.h"
#include "PacketBINARY.h"
#include "PowerControl.h"

// Payload used for PacketUART transmission
uint8_t payload[243];

// Grab the address from the Arduino EEPROM 
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);


unsigned int i; // generic counter
unsigned int pMode;

// string used for PacketUART
String s; 

// length of payload (used in PacketUART) 
int len;


schema_health health;

// count number of samples taken
long sample_counter = 0; 

// global timers
unsigned long transmit_timer = 0; 
unsigned long health_transmit_timer = 0;

LowPassFilter battery_filter;
LowPassFilter solar_filter;
long battery_sample = 0; 
long solar_sample = 0;

long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);

void configureWDT(void);
void enterSleep(void);

void configurePins(void);

void transmitPacketHello(void);
void sampleANDtransmit(void);
void configurePins(void);

void sendDebugPacket(char *dtext);
void transmitDebug(void);

void watch_serial(void);
void run_command(char command);
void barebones_routine(void);
#endif
