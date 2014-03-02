/*
A2AExample_Receiver.pde

This program keep listening to incoming messages, and print received 
messages to serial port.
*/
#include <XBee.h>

XBee xbee = XBee();

// response object to handle incomming messages 
ZBRxResponse ZBrx = ZBRxResponse();
long time = 0;
  
void setup() {
  Serial.begin(9600);
  xbee.begin(Serial);
}

void loop() 
{
  // Start reading
  xbee.readPacket();

  // Checks if there is a packet
  if (xbee.getResponse().isAvailable()) 
  {
    // Checks if the packet is a rx packet
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) 
    {
      // Reads and displays values
      xbee.getResponse().getZBRxResponse(ZBrx);
      for (int i = 0; i < ZBrx.getDataLength (); i++)
        Serial.println(ZBrx.getData(i));

      // Displays size of packet received
      Serial.print("\nSize of Packet: ");
      Serial.println(ZBrx.getDataLength());
      Serial.print("\nTime since last transmission: ");
      Serial.println((millis() - time)/1000);
      Serial.println();
      time = millis();
    }
  }
}







