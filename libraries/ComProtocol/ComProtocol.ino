#define _SOME_HEADER 0xAA
#define _WP_HEADER 0xBB

#include <XBee.h>

#include "def.h"

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0X0);    //desination addr.  0,0 for coord.


expPk expectedPk;





void setup()
{
 Serial.begin(9600);
 xbee.begin(Serial);
 
 initExpPk(&expectedPk);
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
