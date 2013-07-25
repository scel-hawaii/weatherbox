#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>

#include <Adafruit_GPS.h>

#include <Time.h>

#include <string.h>		// memcpy

#include <EEPROM.h>

#include "schema.h"
#include "apple_23.h"
#include "gps_binary_synchronized.h"

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#define XBEE_TRANSMIT_CODE

Adafruit_BMP085 bmp085;

#ifdef XBEE_TRANSMIT_CODE
// SoftwareSerial xbee_softserial(2, 3);
XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);
#endif

SoftwareSerial mySerial(5, 4);	// rx, tx
Adafruit_GPS gps(&mySerial);

uint8_t rf_payload[243];

// Read the address from the EEProm
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);        

schema_1 packet;

unsigned long timer_ms = millis();
unsigned long transmit_ms = 0;
unsigned long sample_ms = 0;

unsigned long sleep_ms = 250;
unsigned long sample_interval_ms = 1000;
unsigned long transmit_hold_ms = 5000;

volatile int f_wdt=1; // for the WDT timer

long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);
void configureWDT(void);
void enterSleep(void);
void configurePins(void);
void setup() {
    Serial.begin(9600);
    #ifdef XBEE_TRANSMIT_CODE
    xbee.begin(Serial);
    #else
    #endif

    // GPS! 
    gps.begin(9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

    bmp085.begin();

    clear_packet();
    packet.gps_valid_sats = 0;

    // replaces Adafruit_GPS "useInterrupt" shenanigans
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);

    // Configure the WDT for sleep
    configureWDT();
    configurePins();
}


void loop()
{
    if(f_wdt == 1)
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
            // power. HoweVER, allow the clock to be scooted backwards up
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

        sample();
        transmit();
        clear_packet();
        
        delay(250);
        /* Adam's old transmit/sample code
        // Cheesy timer wrap-around handling
        if (timer_ms > millis())
        {
            timer_ms = millis();
            transmit_ms = timer_ms;
            sample_ms = timer_ms;
        }

        // Sample every second (250ms sleep 80ms uptime?)
        if (millis() >= (timer_ms/sleep_ms)*sleep_ms + sleep_ms)
        { 
            timer_ms = millis();
            
            // fixme: fixed 1s sampling rate tied to 250ms sleep
            if (timer_ms > sample_ms + 800)
            {
                sample_ms = timer_ms;
                sample();
                //transmit();
            }

            // Transmit every minute
            if (now() % 60 == 0 && 
                timer_ms > transmit_ms + transmit_hold_ms)
            {
                transmit_ms = timer_ms;
                transmit();
                clear_packet();
            }
        }
        */

        delay(100);
        f_wdt = 0;
        enterSleep();
    }
}

void sample()
{
    if (packet.n < 60)
    {
        int n = packet.n;
        packet.uptime_ms = millis();

        int i; float batt_mv_raw = 0, panel_mv_raw = 0, apogee_raw = 0;
        // Find the averages of these values 
        for (i = 0; i < 10; i += 1)
        {
            batt_mv_raw += analogRead(_PIN_BATT_V)*5000.0/1023;
            panel_mv_raw += 2*analogRead(_PIN_SOLAR_V)*5000.0/1023;
            apogee_raw += analogRead(_PIN_APOGEE_V)*5000.0/1023;
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
    #ifdef XBEE_TRANSMIT_CODE
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
    #endif
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
// Configure the inital pin states

void configurePins(){
    #ifdef ANEMOMETER
    pinMode(_PIN_ANEMOMETER0, INPUT);
    pinMode(_PIN_ANEMOMETER1, OUTPUT);
    pinMode(_PIN_ANEMOMETER2, OUTPUT);
    pinMode(_PIN_ANEMOMETER3, OUTPUT);
    #endif

    #ifndef ANEMOMETER
    pinMode(_PIN_ANEMOMETER0, OUTPUT);
    pinMode(_PIN_ANEMOMETER1, OUTPUT);
    pinMode(_PIN_ANEMOMETER2, OUTPUT);
    pinMode(_PIN_ANEMOMETER3, OUTPUT);
    pinMode(9, OUTPUT);
    digitalWrite(9, LOW); 
    digitalWrite(_PIN_ANEMOMETER0, LOW); 
    digitalWrite(_PIN_ANEMOMETER1, LOW); 
    digitalWrite(_PIN_ANEMOMETER2, LOW); 
    digitalWrite(_PIN_ANEMOMETER3, LOW); 
    #endif

    
    pinMode(_PIN_XBEE_SLEEP, OUTPUT);
    digitalWrite(_PIN_XBEE_SLEEP, LOW);

    pinMode(_PIN_PSWITCH, OUTPUT);
    digitalWrite(_PIN_PSWITCH, HIGH);
    
    pinMode(_PIN_SOLAR_V, INPUT);
    pinMode(_PIN_APOGEE_V, INPUT);
    pinMode(_PIN_BATT_V, INPUT);
}


// replaces Adafruit_GPS "useInterrupt" shenanigans
SIGNAL(TIMER0_COMPA_vect) {
    char c = gps.read();
}

/***************************************************
 *  Name:        configureWDT
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: configured the Watch Dog timer for the
                    appropriate settings
 ***************************************************/
void configureWDT(void){
    /* Clear the reset flag. */
    MCUSR &= ~(1<<WDRF);

    /* In order to change WDE or the prescaler, we need to
     * set WDCE (This will allow updates for 4 clock cycles).
     */
    WDTCSR |= (1<<WDCE) | (1<<WDE);

    /* set new watchdog timeout prescaler value */
    //WDTCSR = 1<<WDP2 | 1<<WDP1; /* 1.0 seconds */
    //WDTCSR = 1<<WDP3; /* 4.0 seconds */
    WDTCSR = 1<<WDP3 | 1<<WDP0; /* 8.0 seconds */

    /* Enable the WD interrupt (note no reset). */
    WDTCSR |= _BV(WDIE);

}


/***************************************************
 *  Name:        ISR(WDT_vect)
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 ***************************************************/
ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    // softserial.println("WDT Overrun!!!");
  }
}

/***************************************************
 *  Name:        enterSleep
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: Enters the arduino into sleep mode.
 ***************************************************/
void enterSleep(void)
{
  // set_sleep_mode(SLEEP_MODE_PWR_SAVE);   /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. */
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}
