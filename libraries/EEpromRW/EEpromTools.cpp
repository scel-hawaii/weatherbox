#include "EEpromTools.h"

extern SoftwareSerial mySerial;


/*****************************************************************

  Function name:        initEEAddr

  DESCRIPTION:          This function initializes all the values
                        of the EEprom address struct.

  Parameters:           val - a struct holding the EEprom address
                        attributes.

  Return values:        none

****************************************************************/
void initEEAddr(EEpromAddr* val)
{
 val->low = 0;
 val->high = 0;
 val->finalA = 0; 
}


/*****************************************************************

  Function name:        incEEAddr

  DESCRIPTION:          This function increments the EEprom Address.

  Parameters:           val - a struct holding the EEprom address
                        attributes.

  Return values:        none

****************************************************************/
void incEEAddr(EEpromAddr* val)
{
 if (val->low == 255)
 { val->high++;
   val->low = 1;
 }
 
 else
 val->low++;
}



/*****************************************************************

  Function name:        writeEE

  DESCRIPTION:          This function writes a given byte to the
                        address specified.
                        
                        A slight delay is necessary to ensure that
                        data has been written successfully.

  Parameters:           val - a struct holding the EEprom address
                        attributes.
                        data - the byte to be written.

  Return values:        none

****************************************************************/
void writeEE(EEpromAddr val, uint8_t data)
{
 Wire.beginTransmission(_device_addr);
 Wire.write(val.high);
 Wire.write(val.low);
 Wire.write(data);
 Wire.endTransmission(_device_addr);
 delay(5);
}



/*****************************************************************

  Function name:        readEE

  DESCRIPTION:          This function returns the byte stored at
                        the specified EEprom address.

  Parameters:           val - a struct holding the EEprom address
                        attributes.

  Return values:        The byte stored at the specified address.

****************************************************************/
uint8_t readEE(EEpromAddr val)
{
 Wire.beginTransmission(_device_addr);
 Wire.write(val.high);
 Wire.write(val.low);
 Wire.endTransmission();
 Wire.requestFrom(_device_addr, 1);
 return Wire.read();
}



/*****************************************************************

  Function name:        printData

  DESCRIPTION:          This function prints an EEprom address and
                        a byte that has been passed to it.  Solely
                        for debugging.

  Parameters:           val - a struct holding the EEprom address
                        attributes.
                        data - byte to be printed.

  Return values:        none

****************************************************************/
void printData(EEpromAddr val)
{
 mySerial.print(readEE(val), HEX);
 mySerial.print(" Address   ");
 mySerial.print(int(val.high));
 mySerial.print("     ");
 mySerial.println(int(val.low)); 
}
