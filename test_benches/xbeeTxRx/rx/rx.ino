

#define _start_byte 126


uint8_t Rx[300];
uint8_t byte_position;
uint8_t current_byte;
uint16_t payload_size;
uint32_t checksum;
boolean correct_pk;

uint8_t dummy;



void setup()
{
  Serial.begin(9600);
  
  //initialize start of array to packet header
  Rx[0] = 126;
}


void loop()
{ 
 
  //reinitialize values
  byte_position = 0; 
  payload_size = 0;
  checksum = 0;
  correct_pk = false;
  
  if (Serial.available() > 0)
  {
    //if the current byte isn't the start of a packet,
    //read the next byte
    if (current_byte != _start_byte)
      current_byte = Serial.read();
    
    //continue if byte was start of packet
    if (current_byte == _start_byte)
    { 
      //keep reading packet
      while(1)
      { 
        //if a byte is available, increment
        //counter and read
        if (Serial.available()>0)
        {
          byte_position = byte_position + 1;
          current_byte = Serial.read();
        }
        
        //otherwise, wait for new byte
        else
          continue;
      
        //restart if a start byte was read
        if (current_byte == _start_byte)
          break;
      
        //check if next byte needs to be XOR'd
        if (current_byte == 125)
        {
            //wait for next byte
            while (Serial.available()==0)
            { //do nothing
              //leave brackets or it won't work
            }
            
            //write XOR'd value to array
            Rx[byte_position] = Serial.read() ^ 0x20;
        }
          
        //otherwise, write unaltered value to array 
        else
          Rx[byte_position] = current_byte;
      
        //if at first length byte, add to payload size
        if (byte_position == 1)
          payload_size = 256 * Rx[1];
      
        //if at second length byte, add to payload size
        else if (byte_position == 2)
          payload_size = payload_size + Rx[2];
                    
        //check if packet is done
        if (byte_position == payload_size + 3)
        {
          //calculate checksum
          for (int i=3; i < byte_position; i++)
            checksum = Rx[i] + checksum;
          
          checksum = 0xFF - (checksum % 256);
          
          //compare calculated to given checksum
          if ((uint8_t) checksum == Rx[byte_position])
            correct_pk = true;
            
          break;
        }
      }
    }
  }
  
  //print data if the checksum is correct
  if(correct_pk)
  {
    for (int i=15; i < byte_position; i++)
      Serial.println(Rx[i]);
  }
}
