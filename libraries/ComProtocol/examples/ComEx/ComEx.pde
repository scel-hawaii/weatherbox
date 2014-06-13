/*
    This sketch shows how to use the functions
    in the com tools library.  An updated com
	protocol can be found in the library folder
*/

#include <XBee.h>
#include <ComTools.h>

//the following two lines are for softSerial use with BB
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 8); // RX, TX

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse zb= ZBRxResponse();
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0X0);    //desination addr.  0,0 for coord.


expPk expectedPk;





void setup()
{
 //Serial.begin(9600);
 //xbee.begin(Serial);
 
 mySerial.begin(9600);
 xbee.begin(mySerial);
 
 initExpPk(&expectedPk);
}


void loop()
{
  
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable())
  { //do something if this is a zb rx packet
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
	{
		//fill in values
		xbee.getResponse().getZBRxResponse(zb);
		
		handleXBeeRx(zb, &expectedPk);
		
		//request the next expected packet
		requestPacket(xbee, addr64, expectedPk);
	}
  }
  
}