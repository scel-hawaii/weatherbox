void initAddr(eepromAddr* val)
{
 val->high = 0;
 val->low = 0;
 val->final = 0;
}

void incrementAddr(eepromAddr* val)
{
  if ( val->low != 255)
    val->low = val->low++;
    
  else
  {
    val->high = val->high++;
    val->low = 1; 
  }
}

void initExpPk(expPk* val)
{
 val->header = 0;
 val->nextPk = 0;
 val->totalPk = 0;
}




void storeData(uint8_t *data, uint8_t length)
{  
  for (int i = 3; i < length; i++)
  {
    Serial.println(data[i]);
  }
}



void storeWPData(uint8_t *data, uint8_t length)
{  
    //store .hex fragment
    for (int i = 3; i < length; i++)
    { //hangs here if no EE connected
      writeEE(writeAddr, data[i]);      
      incrementAddr(&writeAddr);
    }
}




void requestPacket(expPk val)
{
  uint8_t payload[3];
  payload[0] = val.header;
  payload[1] = (val.nextPk >> 8);
  payload[2] = (val.nextPk & 0xFF);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  xbee.send(zbTx);
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
    initExpPk(&expectedPk);
  }
  
  //request the next expected packet
  requestPacket(expectedPk);
    
}






void writeEE(eepromAddr val, uint8_t data)
{
 Wire.beginTransmission(_device_addr);
 Wire.write(val.high);
 Wire.write(val.low);
 Wire.write(data);
 Wire.endTransmission(_device_addr);
}


uint8_t readEE(eepromAddr val)
{
 Wire.beginTransmission(_device_addr);
 Wire.write(val.high);
 Wire.write(val.low);
 Wire.endTransmission();
 Wire.requestFrom(_device_addr, 1);
 return Wire.read();
}

