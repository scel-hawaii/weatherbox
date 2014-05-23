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
      Serial.print('k');
}



void storeWPData(uint8_t *data, uint8_t length)
{  
    //store .hex fragment
      for (int i = 3; i < length; i++)
      { //hangs here if no EE connected
        //writeEE(writeAddr, data[i]);      
        
        //incrementAddr(&writeAddr);
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








void print_data(eepromAddr val, uint8_t data)
{
 Serial.print("\n"); 
 Serial.print(data, HEX);
 Serial.print(" Address   ");
 Serial.print(int(val.high));
 Serial.print("     ");
 Serial.println(int(val.low)); 
}
