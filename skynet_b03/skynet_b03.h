/****************************************************
 *
 *          skynet_b03.h
 *          REIS Weatherbox Firmware b03
 *          
 *          The main driver header file for the REIS weatherbox. 
 *
 ****************************************************/
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>

#include <EEPROM.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <SHT1x.h>
#include <OneWire.h>
#include <DallasTemperature.h>

long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);

void configureWDT(void);
void enterSleep(void);

void configurePins(void);

void transmitPacketHello(void);
