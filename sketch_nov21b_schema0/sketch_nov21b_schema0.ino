#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>

Adafruit_BMP085 bmp085;

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t rf_payload[243];

long address = 14159; // fixme: read from eeprom
long batt_mv, panel_mv;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;

long start_ms;

typedef struct
{
    uint16_t schema;
    uint16_t address;
    uint32_t uptime_ms;
    int16_t bmp085_temp_decic;
    uint32_t bmp085_press_pa;
    uint16_t batt_mv;
    uint16_t panel_mv;
    uint16_t apogee_mv;
    uint16_t apogee_w_m2;
} schema_0;

schema_0 packet;

void setup()
{
    start_ms = millis();
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin();
    packet.address = address;
    packet.schema = 0;
}

void loop()
{
    packet.uptime_ms = millis();
    packet.bmp085_temp_decic = bmp085.readTemperature()*10;
    packet.bmp085_press_pa = bmp085.readPressure();
    packet.batt_mv = analogRead(0)*5000.0/1023;
    packet.panel_mv = 2*analogRead(1)*5000.0/1023;
    packet.apogee_mv = (1/15.24)*analogRead(2)*5000.0/1023;
    packet.apogee_w_m2 = apogee_mv*5.0;

    analogWrite(13, 255);
    transmit();
    analogWrite(13, 0);

    while (millis() - start_ms < 1000)
	;

    start_ms = millis();
}

void transmit()
{
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
}
