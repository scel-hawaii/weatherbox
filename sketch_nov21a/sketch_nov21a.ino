#include <SoftwareSerial.h>
#include <XBee.h>

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t payload[243];

long address = 544; // fixme: read from eeprom
boolean have_batt_mv, have_panel_mv;
long batt_mv, panel_mv;

void setup()
{
  softserial.begin(9600);
  xbee.begin(softserial);
}

void loop()
{
  String s = "{";
  s += "\"address\": ";
  s += String(address);
  s += ", \"uptime_ms\": ";
  s += String(millis());
  s += "}";
  s += "blah blah blah";

  for (int i = 0; i < sizeof(payload); i++)
    payload[i] = i % 10 + '0';
  for (int i = 0; i < sizeof(payload) && s[i] != '\0'; i++)
    payload[i] = s[i];

  analogWrite(13, 255);
  ZBTxRequest zbTx = ZBTxRequest(addr64, payload, sizeof(payload));
  xbee.send(zbTx);
  analogWrite(13, 0);
  delay(1000);
}

