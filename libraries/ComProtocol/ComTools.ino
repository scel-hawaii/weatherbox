/*****************************************************************

  Function name:        initExpPk

  DESCRIPTION:          This function initializes all the values
                        of the expected packet struct.

  Parameters:           val - a struct holding the expected packet's
                        attributes.

  Return values:        none

****************************************************************/
void initExpPk(expPk* val)
{
 val->header = 0;
 val->nextPk = 0;
 val->totalPk = 0;
}

/*****************************************************************

  Function name:        storeData

  DESCRIPTION:          This function is a placeholder.
  
                        Later, this function should be used to
                        store data at the appropriate location.

  Parameters:           data - a pointer to the recieved packet
                        length - the length of the packet

  Return values:        none

****************************************************************/
void storeData(uint8_t *data, uint8_t length)
{  
  for (int i = 3; i < length; i++)
  {
    Serial.println(data[i]);
  }
}

/*****************************************************************

  Function name:        requestPacket

  DESCRIPTION:          This function will request a packet with a
                        given header and packet number.

  Parameters:           val - a struct holding the expected packet's
                        attributes.

  Return values:        none

****************************************************************/
void requestPacket(expPk val)
{
  uint8_t payload[3];
  payload[0] = val.header;
  payload[1] = (val.nextPk >> 8);
  payload[2] = (val.nextPk & 0xFF);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  xbee.send(zbTx);
}

/*****************************************************************

  Function name:        handleXBeeRx

  DESCRIPTION:          This function verifies that the received
                        packet was the expected packet.  If it is,
                        it calls the appropriate function to extract
                        data and then increments the expected packet
                        counter.  Then, a request for the next
                        expected packet is made.
                        
                        Once the final packet is reached, values
                        are reinitialized and the appropriate action
                        relating to the packet header will be executed.

  Parameters:           none

  Return values:        none

****************************************************************/
void handleXBeeRx()
{ 
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
      
    //  else if ( rx.getData(0) == _WP_HEADER )
      //  storeData(rx.getData(), rx.getDataLength());
        
      expectedPk.nextPk++;
    }
  }
  
  //cover all cases for final packets
  //set and reset designated values
  if ( expectedPk.nextPk == expectedPk.totalPk + 1)
  {
    initExpPk(&expectedPk);
  }
  
  //request the next expected packet
  requestPacket(expectedPk);
    
}

