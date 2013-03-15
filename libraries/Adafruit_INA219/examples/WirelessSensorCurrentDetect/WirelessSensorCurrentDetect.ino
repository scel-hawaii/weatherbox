#include <Wire.h>
#include <Adafruit_INA219.h>
Adafruit_INA219 ina219_Solar;
Adafruit_INA219 ina219_Battery(0x41);

void setup(void) 
{
  uint32_t currentFrequency;
    
  Serial.begin(115200);
  Serial.println("Hello!");
  
  Serial.println("Measuring voltage and current with INA219 ...");
  ina219_Solar.begin();
  ina219_Battery.begin();
}

void loop(void) 
{
  float shuntvoltage_Solar = 0;
  float busvoltage_Solar = 0;
  float current_mA_Solar = 0;
  float loadvoltage_Solar = 0;
  
  float shuntvoltage_Battery = 0;
  float busvoltage_Battery = 0;
  float current_mA_Battery = 0;
  float loadvoltage_Battery = 0;

  shuntvoltage_Solar = ina219_Solar.getShuntVoltage_mV();
  busvoltage_Solar = ina219_Solar.getBusVoltage_V();
  current_mA_Solar = ina219_Solar.getCurrent_mA();
  loadvoltage_Solar = busvoltage_Solar + (shuntvoltage_Solar / 1000);
  
  shuntvoltage_Battery = ina219_Battery.getShuntVoltage_mV();
  busvoltage_Battery = ina219_Battery.getBusVoltage_V();
  current_mA_Battery = ina219_Battery.getCurrent_mA();
  loadvoltage_Battery = busvoltage_Battery + (shuntvoltage_Battery / 1000);
  
  Serial.print("Solar Bus Voltage:   "); Serial.print(busvoltage_Solar); Serial.println(" V");
  Serial.print("Solar Shunt Voltage: "); Serial.print(shuntvoltage_Solar); Serial.println(" mV");
  Serial.print("Solar Load Voltage:  "); Serial.print(loadvoltage_Solar); Serial.println(" V");
  Serial.print("Solar Current:       "); Serial.print(current_mA_Solar); Serial.println(" mA");
  Serial.println("");
  
  Serial.print("Battery Bus Voltage:   "); Serial.print(busvoltage_Battery); Serial.println(" V");
  Serial.print("Battery Shunt Voltage: "); Serial.print(shuntvoltage_Battery); Serial.println(" mV");
  Serial.print("Battery Load Voltage:  "); Serial.print(loadvoltage_Battery); Serial.println(" V");
  Serial.print("Battery Current:       "); Serial.print(current_mA_Battery); Serial.println(" mA");
  Serial.println("");

  delay(2000);
}
