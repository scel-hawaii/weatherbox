#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>


#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>

#include <QueueList.h>

Adafruit_BMP085 bmp085;

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t payload[243];

long address = 545; // fixme: read from eeprom
long batt_mv, panel_mv;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;

SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  true


// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy


void setup()
{
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin();

    // 9600 NMEA is the default baud rate for Adafruit MTK GPS's- some use 4800
    GPS.begin(9600);


    // uncomment this line to turn on RMC (recommended minimum) and GGA (fix data) including altitude
    GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);

    // Set the update rate
    GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate

    // the nice thing about this code is you can have a timer0 interrupt go off
    // every 1 millisecond, and read data from the GPS for you. that makes the
    // loop code a heck of a lot easier!
    useInterrupt(true);

    delay(1000);
    // Ask for firmware version
    mySerial.println(PMTK_Q_RELEASE);
}

void loop()
{

    QueueList <String> lineToTheMovies;
    String popcornContainer[10];
    

    // Wait for 5 readings before we push out the data
    for( int i = 0 ; i < 10 ; i++)
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
        s += "}";

        s += "       ";		// it explodes without something here... omg wtf

        /*********** GRAB GPS ***********/

        if (GPS.newNMEAreceived()) {
            // a tricky thing here is if we print the NMEA sentence, or data
            // we end up not listening and catching other sentences! 
            // so be very wary if using OUTPUT_ALLDATA and trytng to print out data
            //Serial.println(GPS.lastNMEA());   // this also sets the newNMEAreceived() flag to false

            // this also sets the newNMEAreceived() flag to false
            if (!GPS.parse(GPS.lastNMEA()))   
              // we can fail to parse a sentence in which 
              // case we should just wait for another
              return;  
        }

        /**** GPS VALUES ARE READY ********/
        /*
            All values below are listed as unsigned chars (byte data)
            To convert them, cast them using (int)
            ------
            GPS.hour
            GPS.minute
            GPS.seconds
            GPS.milliseconds
            GPS.day
            GPS.month
            GPS.year
            GPS.fix
            GPS.fixquality
            GPS.lat
            GPS.lon
            GPS.speed
            GPS.angle
            GPS.altitude
            GPS.sattelites

        */
        popcornContainer[i] = s;
        delay(14800);
    }

    // s += "blah blah blah";

    // s += "Hath not a Jew eyes? Hath not a Jew hands, organs, dimensions, senses, affections, passions, fed with the same food, hurt with the same weapons, subject to the same diseases, healed by the same means, warmed and cooled by the same winter and summer, as a Christian is? If you prick us, do we not bleed? If you tickle us, do we not laugh? If you poison us, do we not die? And if you wrong us, shall we not revenge?"; // s misbehaves when long

    /* Empty the stack */
    
    // String popcornContainer[10]
    String s;
    for(int k = 0 ; k < 5 ; k ++)
    {
        s = popcornContainer[k];

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
    }



}



/*******************************************************************
 * 
 *                  GPS Fetching Functions
 *
 *
 *******************************************************************/

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

uint32_t timer = millis();
