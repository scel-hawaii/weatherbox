#include <Arduino.h>
#include "dac.h"

Adafruit_MCP4725 dac;

void DAC_initDac(void){
    dac.begin(0x62);
}

void DAC_setVoltage(float voltage){
    // Convert the voltage value to a digital value
    int voltage_digital = 0;
    voltage_digital = (int) ((voltage/5)*4095);
    DAC_setVoltageDigital(voltage_digital);
}

void DAC_setVoltageDigital(int voltage){
    dac.setVoltage(voltage, false);
}
