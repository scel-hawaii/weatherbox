#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SHT1x.h>
#include "TSL2561.h"
#include <Adafruit_INA219.h>


#include <SoftwareSerial.h>
#include <XBee.h>

Adafruit_BMP085 bmp085;

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x408d9dd0);

//Humidity
#define SHTdataPin  7
#define SHTclockPin 8
SHT1x sht1x(SHTdataPin, SHTclockPin);

//Temperature
#define ONE_WIRE_BUS 6
OneWire oneWire(ONE_WIRE_BUS); // OneWire bus for Dallas temperature sensor
DallasTemperature ds20(&oneWire); // Dallas temperature sensor
// Device addr (read from library method to discover addrs)
DeviceAddress Thermometer = { 0x28, 0xD1, 0xD4, 0xCE, 0x03, 0x00, 0x00, 0x64 };
TSL2561 tsl(TSL2561_ADDR_FLOAT);  // Light sensor

// Current Sensors
Adafruit_INA219 ina219_Solar;
Adafruit_INA219 ina219_Battery(0x41);

uint8_t payload[243];

long address = 13; // fixme: read from eeprom
long batt_mv, panel_mv;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;

long ds20_temp_decic; // constant 2 decimals of precicsionsdfsdf

long inasolarbus_v, inasolarshunt_mv, inasolarcurr_ma; // Also has constant 2 decimals of precision
long inabattbus_v, inabattshunt_mv, inabattcurr_ma;

long tsl_w_m2; // Assumed to have no fractional values per justin

long sht1x_humid_pct; // 2 decimals of precision

void setup()
{
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin(); // BMP multisensor
    
    ds20.begin(); // Dallas temp sensor
    ds20.setResolution(Thermometer, 10); // Sets temp resolution to 10 bit (play with later)
    
    // Light Sensors
    tsl.begin();
    // You can change the gain on the fly, to adapt to brighter/dimmer light situations
    tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
    //tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)  
    tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light) 
    
    ina219_Solar.begin();
    ina219_Battery.begin();
}

void loop()
{
    String s = "{";
    s += "\"addr\": ";
    s += String(address);
    s += ", \"up_ms\": ";
    s += String(millis());
    s += ", \"bmp_c\": ";
    bmp085_temp_decic = bmp085.readTemperature()*10;
    s += String(bmp085_temp_decic);
    s += ", \"bmp_pa\": ";
    bmp085_press_pa = bmp085.readPressure();
    s += String(bmp085_press_pa);
//    s += ", \"batt_mv\": ";
//    batt_mv = analogRead(0)*5000.0/1023;
//    s += String(batt_mv);
//    s += ", \"panel_mv\": ";
//    panel_mv = 2*analogRead(1)*5000.0/1023;
//    s += String(panel_mv);
//    s += ", \"apogee_mv\": ";
//    apogee_mv = (1/15.24)*analogRead(2)*5000.0/1023;
//    s += String(apogee_mv);
//    s += ", \"apogee_w_m2\": ";
//    apogee_w_m2 = apogee_mv*5.0; Why the hell are there two of these anyway?
//    s += String(apogee_w_m2);
    
    s += ", \"sht_pct\": ";
    sht1x_humid_pct = sht1x.readHumidity()*100;
    s += String(sht1x_humid_pct);

    // Temperature added here
    s += ", \"ds20_c\": ";
    ds20_temp_decic = ds20.getTempC(Thermometer)*100; // we don't need no stinkin fahrenheit
    s += String(ds20_temp_decic);
    
    // TSL Light sensor added here (Note that the TSL sensor has multiple frequency band options in its methods.)
    s += ", \"tsl_m2\": ";
    tsl_w_m2 = tsl.getFullLuminosity();
    s += String(tsl_w_m2);
    
    // Current sensors added here
    s += ", \"solar_v\": ";
    inasolarbus_v = ina219_Solar.getBusVoltage_V()*100;
    s += String(inasolarbus_v);
    s += ", \"shunt_mv\": ";
    inasolarshunt_mv = ina219_Solar.getShuntVoltage_mV()*100;
    s += String(inasolarshunt_mv);
    s += ", \"solar_ma\": ";
    inasolarcurr_ma = ina219_Solar.getCurrent_mA()*100;
    s += String(inasolarcurr_ma);
    s += ", \"batt_v\": ";
    inabattbus_v = ina219_Battery.getBusVoltage_V()*100;
    s += String(inabattbus_v);
    s += ", \"batt_mv\": ";
    inabattshunt_mv = ina219_Battery.getShuntVoltage_mV()*100;
    s += String(inabattshunt_mv);
    s += ", \"batt_ma\": ";
    inabattcurr_ma = ina219_Battery.getCurrent_mA()*100;
    s += String(inabattcurr_ma);
    
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
    analogWrite(13, 0);
    delay(14800);
    Serial.println(s);
}
