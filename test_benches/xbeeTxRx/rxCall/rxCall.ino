#include <avr/wdt.h>

#define _start_byte 126
#define _XOR_marker 125
#define _length_byte1 1
#define _length_byte2 2


uint8_t Rx[300];
uint8_t byte_position;
uint8_t current_byte;
uint16_t payload_size;
uint32_t checksum;
boolean correct_pk;

void read_pk();



void setup()
{
  Serial.begin(9600);
  
  Serial.println ("Restarted.");
  wdt_enable (WDTO_1S);  // reset after one second, if no "pat the dog" received
}


void loop()
{ 

wdt_reset ();  // give me another second to do stuff (pat the dog)  
  if (Serial.available() > 0)
    read_pk();
}


void read_pk()
{ 
 
  //initialize values
  byte_position = 0; 
  payload_size = 0;
  checksum = 0;
  Rx[0] = _start_byte;
  correct_pk = false;
  
  //if the current byte isn't the start of a packet,
  //trash data until the start of a packet is reached
  while (current_byte != _start_byte)
    current_byte = Serial.read();
    
    
  while(1)
  { 
    //increment counter and read data
    if (Serial.available()>0)
    {
      byte_position = byte_position + 1;
      current_byte = Serial.read();
    }
        
    //wait for new data if none was read
    else
      continue;
      
    //restart if a start byte was read
    if (current_byte == _start_byte)
      break;
    
    
      
    //write next byte to array
    if (current_byte == _XOR_marker)
    {
      while (Serial.available()==0)
      {  //do nothing until the next byte is available
      }
            
      Rx[byte_position] = Serial.read() ^ 0x20;
    }

    else
      Rx[byte_position] = current_byte;
    
    
      
    //calculate payload length if at length bytes
    if (byte_position == _length_byte1)
      payload_size = 256 * Rx[1];
      
    else if (byte_position == _length_byte2)
      payload_size = payload_size + Rx[2];
    
    
                    
    //calculate and compare checksum if the
    //end of the packet is reached
    if (byte_position == payload_size + 3)
    {
      for (int i=3; i < byte_position; i++)
        checksum = Rx[i] + checksum;
          
      checksum = 0xFF - (checksum % 256);
          
      if ((uint8_t) checksum == Rx[byte_position])
        correct_pk = true;
      
      break;
    }
  }
  
  //print data if the checksum is correct
  if(correct_pk)
  {
//    for (int i=15; i < byte_position; i++)
//      Serial.println(Rx[i]);

Serial.println("Got the packet.");
  }
}
