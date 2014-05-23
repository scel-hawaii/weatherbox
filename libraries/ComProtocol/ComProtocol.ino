#define _device_addr B01010000

#define _SOME_HEADER 0xAA
#define _WP_HEADER 0xBB

#include <Wire.h>
#include <XBee.h>

#include "def.h"

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0X0);    //desination addr.  0,0 for coord.


expPk expectedPk;

eepromAddr readAddr;
eepromAddr writeAddr;

volatile uint8_t RxBuffer[110];




void setup()
{
 Serial.begin(9600);
 xbee.begin(Serial);
 
 initExpPk(&expectedPk);
 initAddr(&readAddr);
 initAddr(&writeAddr);
}


void loop()
{
  
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable())
  { //do something if this is a zb rx packet
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
      handleXBeeRx();
  }
  
}




void handleXBeeRx()
{
  uint8_t header = 0;
  
  //fill in values
  xbee.getResponse().getZBRxResponse(rx);
  
  //do something with data if this is an expected packet
  if ( rx.getData(1) * 256 + rx.getData(2) == expectedPk.nextPk )
  {
    //set values if this is the first packet
    if ( expectedPk.nextPk == 0 )
    {
      expectedPk.nextPk++;
      expectedPk.header = rx.getData(0);
      expectedPk.totalPk = (uint16_t)rx.getData(3) * 256 + rx.getData(4);
    }
    
    else if ( rx.getData(0) == expectedPk.header )
    {
      if ( rx.getData(0) == _SOME_HEADER )
        storeData(rx.getData(), rx.getDataLength());
      
      else if ( rx.getData(0) == _WP_HEADER )
        storeWPData(rx.getData(), rx.getDataLength());
        
      expectedPk.nextPk++;
    }
  }
  
  //cover all cases for final packets
  //set and reset designated values
  if ( expectedPk.nextPk == expectedPk.totalPk + 1)
  {
    readAddr.final = (uint16_t)writeAddr.high * 256 + writeAddr.low - 1;
    initAddr(&writeAddr);
    expectedPk.nextPk = 0;
  }
  
  //request the next expected packet
  else
    requestPacket(expectedPk);
    
}
