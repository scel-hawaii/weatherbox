#include <Wire.h>
#include <XBee.h>

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
ZBRxResponse rx = ZBRxResponse();
XBeeAddress64 addr64 = XBeeAddress64(0x0, 0X0);    //desination addr.  0,0 for coord.


volatile uint8_t RxBuffer[110];



void setup()
{
 Serial.begin(9600);
 xbee.begin(Serial);
 
 initRxBuffer();
}


void loop()
{
  
  xbee.readPacket();
  
  if (xbee.getResponse().isAvailable())
  { //do something if this is a zb rx packet
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
    {
      handleXbeeRxMessage();
    }
    
  }  
}




void handleXbeeRxMessage()
{
  xbee.getResponse().getZBRxResponse(rx); //fill in values
  
  //handle wireless programming packets
  //use rx.getData(i) to read byte at index i
    
  storeData(rx.getData(), rx.getDataLength());
}







void storeData(uint8_t *data, uint8_t length)
{
  int i = 0, n = 0;
  
  //find index of null char
  while ( RxBuffer[n] != '\0' )
  {
    n++;
  }
  
  //store data starting from null char
  for (i = n; i < length + n; i++)
  {
    //Serial.print(data[i-n], HEX);
    RxBuffer[i] = data[i - n];
  }
      
  RxBuffer[i] = '\0';
}



void initRxBuffer()
{
 int i = 0;
 
 for (i = 0; i < 110; i++)
 {
   RxBuffer[i] = '\0';
 }
}
