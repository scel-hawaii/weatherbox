// Sensors.h

#ifndef SENSORS_H
#define SENSORS_H

#include "apple_23.h"
#include <SHT1x.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_INA219.h>
#include <Adafruit_BMP085.h>

OneWire oneWire1(_PIN_AMB_TEMP);
DallasTemperature dallas_amb_sen(&oneWire1);

OneWire oneWire2(_PIN_ROOF_TEMP);
DallasTemperature dallas_roof_sen(&oneWire2);

SHT1x sht1x(_PIN_HUMID_DATA, _PIN_HUMID_CLK);

Adafruit_BMP085 bmp085;
Adafruit_INA219 ina219_Solar;

int Sensors_sampleBatterymV(void);
int Sensors_samplePanelmV(void);
int Sensors_sampleSolarIrrmV(void);
int Sensors_samplePressurepa(void);
int Sensors_sampleHumiditypct(void);
int Sensors_sampleTempdecic(void);
#endif
