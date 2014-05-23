#ifndef DAC_H
#define DAC_H

#include <Arduino.h>
#include <Adafruit_MCP4725.h>

extern Adafruit_MCP4725 dac; 

void DAC_initDac(void);
void DAC_setVoltage(float voltage);
void DAC_setVoltageDigital(int voltage);
void DAC_cool(void);
#endif
