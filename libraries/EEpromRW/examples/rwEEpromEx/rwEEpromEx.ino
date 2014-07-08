/*
  This sketch shows how to use the functions
  in the external EEprom RW libary.  
  
  *print function is configured to work with 
  software serial
*/

#include <Wire.h>
#include <SoftwareSerial.h>
#include <EEpromTools.h>

EEpromAddr readAddr;
EEpromAddr writeAddr;

SoftwareSerial mySerial(3, 9); // RX, TX of prog chip

void setup()
{
 Wire.begin();
 mySerial.begin(9600);
 
 initEEAddr(&readAddr);
 initEEAddr(&writeAddr);
}


void loop()
{
  printData(readAddr);
  incEEAddr(&readAddr);
/*
  writeEE(writeAddr, 0);
  incEEAddr(&writeAddr);
*/
}
