
#include <XBee.h>

#include "schema.h"

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t payload[84];

schema_1 packet;

long time = 0;

#define RQ_XBEE_SLEEP 7

void setup()
{
  Serial.begin(9600);
  xbee.begin(Serial);
    
  pinMode(RQ_XBEE_SLEEP, OUTPUT);
}

void loop()
{
  /*packet.schema = 1;
  packet.address = 0;		// Address of Arduino	
  packet.uptime_ms = 0;		// Time since start of program
  packet.n = 0;			// number of data points in packet 0..30 
  for(int count = 0; count < 7; count++)
  {
    packet.batt_mv[count] = 0;	// Battery Voltage (in milli volts)
    packet.panel_mv[count] = 0;	// Panel Voltage (in milli volts)
    packet.apogee_w_m2[count] = 0;
  }
  packet.bmp085_press_pa = 0;	// Pressure Value (in pascals)
  packet.bmp085_temp_decic = 0;  // Temperature Value (in celsius)
  packet.humidity_centi_pct = 0;
  packet.gps_valid_sats = 0;
  packet.gps_year = 0;
  packet.gps_month = 0;
  packet.gps_day = 0;
  packet.gps_hour = 0;
  packet.gps_minute = 0;
  packet.gps_second = 0;
  packet.gps_lat = 0;
  packet.gps_lon = 0;
  packet.gps_elev = 0;*/
  
  // Initialize packet to 0
  memset(payload, '\0', sizeof(payload));

  // Copy data into packet
  memcpy(payload, &packet, sizeof(packet));

  if((millis() - time) >= 5000)  
  {
      digitalWrite(RQ_XBEE_SLEEP, LOW);
      time = millis();
      while((millis() - time) < 3000);
      time = millis();  
      
      // Transmit Packet
      ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(packet));
      xbee.send(zbTx);
  }
  else
      digitalWrite(RQ_XBEE_SLEEP, HIGH);
}
