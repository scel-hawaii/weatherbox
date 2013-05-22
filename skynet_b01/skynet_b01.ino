#define NODE_ADDRESS 103
// change me!!!11!1
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>

Adafruit_BMP085 bmp085;
Adafruit_INA219 ina219_Solar;

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t payload[243];

long address = NODE_ADDRESS;
long batt_mv, panel_mv, panel_ma;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;
long time = 0;

void setup()
{
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin();
    ina219_Solar.begin();

    time = millis();
}

void loop()
{
    String s = "{";
    s += "\"address\": ";
    s += String(address);
    s += ", \"uptime_ms\": ";
    s += String(millis());
    s += ", \"bmp085_temp_decic\": ";
    bmp085_temp_decic = bmp085.readTemperature()*10;
    s += String(bmp085_temp_decic);
    s += ", \"bmp085_press_pa\": ";
    bmp085_press_pa = bmp085.readPressure();
    s += String(bmp085_press_pa);
    s += ", \"batt_mv\": ";
    batt_mv = analogRead(0)*5000.0/1023;
    s += String(batt_mv);
    s += ", \"panel_mv\": ";
    panel_mv = 2*analogRead(1)*5000.0/1023;
    s += String(panel_mv);
    s += ", \"apogee_mv\": ";
    apogee_mv = (1/15.24)*analogRead(2)*5000.0/1023;
    s += String(apogee_mv);
    s += ", \"apogee_w_m2\": ";
    apogee_w_m2 = apogee_mv*5.0;
    s += String(apogee_w_m2);
    
    s += ", \"panel_ma\": ";
    panel_ma = ina219_Solar.getCurrent_mA()*100;
    s += String(panel_ma);
    s += "}";

    s += "       ";		// it explodes without something here... omg wtf

    // s += "blah blah blah";

    // s += "Hath not a Jew eyes? Hath not a Jew hands, organs, dimensions, senses, affections, passions, fed with the same food, hurt with the same weapons, subject to the same diseases, healed by the same means, warmed and cooled by the same winter and summer, as a Christian is? If you prick us, do we not bleed? If you tickle us, do we not laugh? If you poison us, do we not die? And if you wrong us, shall we not revenge?"; // s misbehaves when long

    for (int i = 0; i < sizeof(payload); i++)
	payload[i] = i % 10 + '0';
    // payload[i] = '\0';

    int len;
    for (int i = 0; i < sizeof(payload) && s[i] != '\0'; len = ++i) // yikes
	payload[i] = s[i];

    analogWrite(13, 255);
    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, len);
    xbee.send(zbTx);
    Serial.println(s);
    analogWrite(13, 0);
   // delay(14800);
    while(millis() - time < 14800);
    time = millis();
}
