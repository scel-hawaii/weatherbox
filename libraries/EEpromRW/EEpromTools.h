
#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>

#define _device_addr 0b01010000


typedef struct {
    
    uint8_t high;
    uint8_t low;
    uint16_t finalA;
} EEpromAddr;




void initEEAddr(EEpromAddr* val);

void incEEAddr(EEpromAddr* val);

void writeEE(EEpromAddr val, uint8_t data);

uint8_t readEE(EEpromAddr val);

void printData(EEpromAddr val);
