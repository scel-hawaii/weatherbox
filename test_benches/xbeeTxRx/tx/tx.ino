#include <XBee.h>

// Create an XBee object
XBee xbee = XBee();

// Create an array for holding the data you want to send.
uint8_t payload[92];

void setup()
{
  Serial.begin(9600);
  
  // Tell XBee to start Serial
  xbee.begin(Serial);
}


void loop()
{ 
  for (int i=0; i <=100; i++)
  {
    
    //fill array
    payload[i] = 126;
    
    //place values in specified location
    if (i==87)
    payload[i] = 125;
    
    if (i == 89)
    payload[i] = 2;
  }

  // Specify the address of the remote XBee (this is the SH + SL)
  // 0x0, 0X0FFFF is for broadcasting
  // 0x0, 0x0 addresses the coordinator
  XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0X0000FFFF);

  // Create a TX Request
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

  // Send your request
  xbee.send(zbTx);
  
  
  delay(2500);
}


//test for all 1's
//transmits for payload[92] correctly
//does not transmit for payload[93]

//test for all 126's
//transmits for payload[
