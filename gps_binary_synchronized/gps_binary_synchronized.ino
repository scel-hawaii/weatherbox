#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>

#include <Adafruit_GPS.h>

#include <Time.h>

#include <string.h>		// memcpy

Adafruit_BMP085 bmp085;

SoftwareSerial xbee_softserial(2, 3);
XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

SoftwareSerial mySerial(5, 4);	// rx, tx
Adafruit_GPS gps(&mySerial);

uint8_t rf_payload[243];

long address = 0;

typedef struct
{
    uint16_t schema;
    uint16_t address;
    uint32_t uptime_ms;
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[15];
    uint16_t panel_mv[15];
    uint32_t bmp085_press_pa;
    int16_t bmp085_temp_decic;
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[60];

    uint8_t gps_valid_sats;
    uint8_t gps_year;
    uint8_t gps_month;
    uint8_t gps_day;
    uint8_t gps_hour;
    uint8_t gps_minute;
    uint8_t gps_second;
    int32_t gps_lat;
    int32_t gps_lon;
    int32_t gps_elev;
} schema_1;

schema_1 packet;

unsigned long timer_ms = millis();
unsigned long transmit_ms = 0;
unsigned long sample_ms = 0;

unsigned long sleep_ms = 250;
unsigned long sample_interval_ms = 1000;
unsigned long transmit_hold_ms = 5000;

void setup()
{
    xbee_softserial.begin(9600);
    xbee.begin(xbee_softserial);

    gps.begin(9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

    bmp085.begin();

    clear_packet();
    packet.gps_valid_sats = 0;

    // replaces Adafruit_GPS "useInterrupt" shenanigans
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
}

// replaces Adafruit_GPS "useInterrupt" shenanigans
SIGNAL(TIMER0_COMPA_vect) {
    char c = gps.read();
}

void loop()
{
    if (gps.newNMEAreceived() && gps.parse(gps.lastNMEA()) && gps.fix)
    {
	if (packet.gps_valid_sats == 0)
	{
	    packet.gps_valid_sats = gps.satellites;
	    packet.gps_year = gps.year;
	    packet.gps_month = gps.month;
	    packet.gps_day = gps.day;
	    packet.gps_hour = gps.hour;
	    packet.gps_minute = gps.minute;
	    packet.gps_second = gps.seconds;
	    packet.gps_lat = gps.latitude * 1e6;
	    packet.gps_lon = gps.longitude * 1e6;
	    packet.gps_elev = gps.altitude;
	}

	// Resynchronize onboard time from GPS. Don't sync time
	// *backwards* -- this happens when GPS loses & regains
	// power. However, allow the clock to be scooted backwards up
	// to six minutes, which is what I expect the Arduino to gain
	// per day worst case.
	tmElements_t tm;
	tm.Hour = gps.hour; tm.Minute = gps.minute; tm.Second = gps.seconds;
	tm.Year = gps.year; tm.Month = gps.month; tm.Day = gps.day;
	// fixme: set GPS location / satellites here
	if (makeTime(tm) > min(now(), now() - 360)) // now() could be 0
	{
	    setTime(gps.hour, gps.minute, gps.seconds, 
		    gps.day, gps.month, gps.year);
	}
    }

    // Cheesy timer wrap-around handling
    if (timer_ms > millis())
    {
	timer_ms = millis();
	transmit_ms = timer_ms;
	sample_ms = timer_ms;
    }

    if (millis() >= (timer_ms/sleep_ms)*sleep_ms + sleep_ms)
    { 
	timer_ms = millis();
    
	// fixme: fixed 1s sampling rate tied to 250ms sleep
	if (timer_ms > sample_ms + 800)
	{
	    sample_ms = timer_ms;
	    sample();
	    // transmit();
	}

	if (now() % 60 == 0 && 
	    timer_ms > transmit_ms + transmit_hold_ms)
	{
	    transmit_ms = timer_ms;
	    transmit();
	    clear_packet();
	}
    }
}

void sample()
{
    if (packet.n < 60)
    {
	int n = packet.n;
	packet.uptime_ms = millis();

	int i; float batt_mv_raw = 0, panel_mv_raw = 0, apogee_raw = 0;
	for (i = 0; i < 10; i += 1)
	{
	    batt_mv_raw += analogRead(0)*5000.0/1023;
	    panel_mv_raw += 2*analogRead(1)*5000.0/1023;
	    apogee_raw += analogRead(2)*5000.0/1023*0.25;
	}
	packet.batt_mv[n/4] = batt_mv_raw/10.0;
	packet.panel_mv[n/4] = panel_mv_raw/10.0;
	packet.bmp085_press_pa = bmp085.readPressure();
	packet.bmp085_temp_decic = bmp085.readTemperature()*10;
	packet.apogee_w_m2[n] = apogee_raw/10.0;
	packet.n += 1;
    }
}

void transmit()
{
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
}

void clear_packet()
{
    packet.schema = 1;
    packet.address = address;
    packet.uptime_ms = 0;
    packet.gps_valid_sats = 0;
    packet.n = 0;
    packet.bmp085_press_pa = 0;
    packet.bmp085_temp_decic = 0;
    packet.humidity_centi_pct = 0;
    int i;
    for (i = 0; i < 60; i += 1)
    {
	packet.batt_mv[i/4] = 0;
	packet.panel_mv[i/4] = 0;
	packet.apogee_w_m2[i] = 0;
    }
}
