/*  -collects 1 sample/second per sensor
      -can be changed by altering the value of the
        delay at the bottom of the loop
        
    -each sample is represented by 2 bytes
      -sent as MsB sample1, LsB sample2, MsB sample2, 
        LsB sample2, Msb sample3, ....

    -values are scaled up by 100 to get rid of decimals
      -to be read as volts, multiply final value by 100
*/



#include <XBee.h>

void xbee_tx();
void fill_payload(uint16_t sample);

XBee xbee = XBee();
uint8_t payload[89];
int payload_count = 0;

int voltPin_cal = 0;
int voltPin_ncal = 1;
int voltPin_apo = 2;



void setup()
{
  Serial.begin(9600);
  
  xbee.begin(Serial);
}



void loop(void)
{

  int sensorValue1 = analogRead(voltPin_cal);
  int sensorValue2 = analogRead(voltPin_ncal);
  int sensorValue3 = analogRead(voltPin_apo);
  
  //multiplies float by 100 to get rid of decimals
  uint16_t voltage_cal = sensorValue1 * (5 / 1024.000) * 100;
  uint16_t voltage_ncal = sensorValue2 * (5 / 1024.000) * 100;
  uint16_t voltage_apo = sensorValue3 * (5 / 1024.000) * 100;
  
  
  //fill the payload and increment counter
  fill_payload(voltage_cal);
  fill_payload(voltage_ncal);
  fill_payload(voltage_apo);
  
  //tx when the payload reaches max capacity
  if (payload_count == 90)
  {  xbee_tx();
     payload_count = 0;
  }
  
  delay(1000);
}






void fill_payload(uint16_t sample)
{
  uint8_t ms_byte, ls_byte;
  
  //seperate numbers into msB and lsB and fill the array
  ms_byte = sample >> 8;
  ls_byte = sample & 255;
  payload[payload_count] = ms_byte;
  payload[payload_count + 1] = ls_byte;

  //increment the payload counter
  payload_count = payload_count + 2;
}
 
 
  
void xbee_tx()
{ // Specify the address of the remote XBee (this is the SH + SL)
  XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0X0000FFFF);

  // Create a TX Request
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));

  // Send your request
  xbee.send(zbTx);
}
