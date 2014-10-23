// ATRequest


#include <XBee.h>
#include <SoftwareSerial.h>
XBee xbee = XBee();
SoftwareSerial softserial(12, 11); //RX, TX

AtCommandRequest request = AtCommandRequest();
AtCommandResponse response = AtCommandResponse();

void setup(){
    Serial.begin(9600);
    xbee.begin(softserial);
    Serial.println("Finish init.");
}

void loop(){
    while(1){
        // uint8_t command[] = {'M', 'Y'};
        // request.setCommand(command);
        // request.clearCommandValue();
        sendAtCommand();
        delay(1000);
    }
}
void sendCoolCommand(void){
    softserial.print(0x7E, HEX); 
    softserial.print(0x00, HEX); 
    softserial.print(0x04, HEX); 
    softserial.print(0x08, HEX); 
    softserial.print(0x01, HEX); 
    softserial.print(0x4D, HEX); 
    softserial.print(0x59, HEX); 
    softserial.print(0x50, HEX); 
}


void sendAtCommand() {
  Serial.print("Sending command to the XBee ");
  
  // send the command
  // xbee.send(request);
  sendCoolCommand();
  Serial.println("");
  
  // wait up to 1 second for the status response
  // we are just using this function for local queries, should be quick
  if (xbee.readPacket(1000)) {
    // got a response!
    Serial.println("We got a response!");
    
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(response);
      
      if (response.isOk()) {
        Serial.print("Command [");
        Serial.print(response.getCommand()[0]);
        Serial.print(response.getCommand()[1]);
        Serial.println("] was successful!");
        
        if (response.getValueLength() > 0) {
          Serial.print("Command value length is ");
          Serial.println(response.getValueLength(), DEC);
          
          Serial.print("Command value: ");
          for (int i = 0; i < response.getValueLength(); i++) {
            Serial.print(response.getValue()[i], HEX);
            Serial.print(" ");
          }
          Serial.println("");
        }
      }
      else {
        Serial.print("Command return error code: ");
        Serial.println(response.getStatus(), HEX);
      }
    } else {
      Serial.print("Expected AT response but got ");
      Serial.println(xbee.getResponse().getApiId(), HEX);
    }
  } else {
    // command failed
    if (xbee.getResponse().isError()) {
      Serial.print("Error reading packet. Error code: ");
      Serial.println(xbee.getResponse().getErrorCode(), DEC);
    }
    else {
      Serial.println("No response from radio");
    }
  }
  Serial.println("");
}
