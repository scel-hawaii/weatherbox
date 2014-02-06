/****************************************************
 *
 *          skynet_b03.ino
 *          REIS Weatherbox Firmware b03
 *          
 *          The main driver file for the REIS weatherbox. 
 *          -------------------------------------
 *          Major Design Changelog (See readme for more):
 *          - Double features: binary packet AND UART packet
 *          - For future "on the fly", live config changes
 *          - Better program flow for live  system config changes
 *          - Confirmed xbee needs to be in router
 *
 ****************************************************/
#include "skynet_b03.h"

/* Can't move libraries into skynet_b03.h. For some reason the program will 
   not compile if that happens. */

// explain what libraries are for and where to find them
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>
#include <Adafruit_GPS.h>

#include <EEPROM.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <SHT1x.h>              // Humidity sensor
#include <OneWire.h>            // 
#include <DallasTemperature.h>  // Temperature sensor

#include "schema.h"
#include "sleep.h"
#include "apple_23.h"

#define FALSE 0
#define TRUE 1

OneWire oneWire1(_PIN_AMB_TEMP);
DallasTemperature dallas_amb_sen(&oneWire1);

OneWire oneWire2(_PIN_ROOF_TEMP);
DallasTemperature dallas_roof_sen(&oneWire2);

SHT1x sht1x(_PIN_HUMID_DATA, _PIN_HUMID_CLK);

Adafruit_BMP085 bmp085;
Adafruit_INA219 ina219_Solar;

SoftwareSerial softserial(9, 10);

SoftwareSerial mySerial(5,4);  // rx,tx (used for gps)
Adafruit_GPS gps(&mySerial);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

// Payload used for PacketUART transmission
uint8_t payload[243];

// Grab the address from the Arduino EEPROM 
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);

long batt_mv, panel_mv, panel_ua;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;
long dallas_rooftemp_c, dallas_ambtemp_c;

unsigned int i; // generic counter
unsigned int pMode;

// string used for PacketUART
String s; 

// length of payload (used in PacketUART) 
int len;

// payload used for PacketBINARY transmission
uint8_t rf_payload[80];

// Use schema_3 for wind sensor, use schema_4 for no wind_sensor
#ifdef WIND_SENSOR
    schema_3 packet;
#else
    schema_4 packet;
#endif
// Declare sampling struct
Sample samp;

// used to count number of samples taken
long sample_counter = 0; 

/***************************************************
 *      setup()
 *
 *      This function is run once at the beginning of the start
 *      sequence, before the setup function. Put any init scripts 
 *      you may need in here. 
 ***************************************************/
void setup() {
    // sets speed for communication
    Serial.begin(9600);

    // GPS!
    gps.begin(9600);
    gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
    gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);

    packet.gps_valid_sats = 0;

    // replaces Adafruit_GPS "useInterrupt" shenanigans
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);

    // initialize packet to send all data sampled
    packet.indicator = 0xFFFF;

    // initialize sampling rates for 6 samples per minute
    samp.battery = 6;
    samp.solar_panel = 6;
    samp.apogee = 6;

    xbee.begin(Serial);
    bmp085.begin();
    ina219_Solar.begin();
    
    // Configuration Scripts
    configurePins();
    configureADC();

    clear_packet();

    // Wait to make sure configuration finishes
    delay(2000);
    transmitPacketHello();  // Say Hello

    configureWDT();
    configureINT();
}

/***************************************************
 *      loop()
 *
 *      This function is the main program function, running
 *      constantly. It is executed after the setup() function, and
 *      continues running indefinately until certain conditions
 *      stop it. Analogous to the main() function in traditional 
 *      C and C++ programs
 ***************************************************/
void loop() {
    // Check the watchdog timer flag 
    if(f_wdt == 1)
    {
        int batteryV = sampleBatteryVoltage();
        if(batteryV > THRESH_GOOD_BATT_V){
            digitalWrite(_PIN_PSWITCH, HIGH);
            sampleANDtransmit();
            delay(100);                             // Delay and wait for transmit to finish
	    					    // Implement interrupt flag instead of delay
        }
        else{
            while(batteryV < 3800) {
                digitalWrite(_PIN_PSWITCH, LOW);    
                batteryV = sampleBatteryVoltage();
                delay(100);                         // Wait 100 ms for everything to settle
						    // sleep for 10 min instead of delay
            }
        }
        f_wdt = 0;
        enterSleep();
    }
}

// replaces Adafruit_GPS "useInterrupt" shenanigans
SIGNAL(TIMER0_COMPA_vect) {
    char c = gps.read();
}

/***************************************************
 *  Name:        sampleANDtransmit
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: One sample and transmit function. This function
 *                  takes care of the logic behind the different modes,
 *                  and executes it when needed.
 *
 *
 ***************************************************/
void sampleANDtransmit(void){
    // Check which method for transmitting packet

#ifdef PACKET_BINARY   // transmits using BINARY packet

            samplePacketBinary();
            sample_counter++;
	    // Check if desired amount of samples for transmit have been taken
            if(sample_counter == _CONFIG_TransmitPeriod) {
	        for(f_wdt = 0;!(gps.newNMEAreceived() && gps.parse(gps.lastNMEA()));)
		{
		   if(f_wdt == 1) break; 
		}
		f_wdt = 1;

                if(gps.fix)
		{
		    packet.gps_hour = gps.hour;
	  	    packet.gps_minute = gps.minute;
		    packet.gps_second = gps.seconds;
		}
		else
		{
		    packet.gps_hour = 255;    // Not a legitimate time
	  	    packet.gps_minute = 255;  // Not a legitimate time
		    packet.gps_second = 255;  // Not a legitimate time
		}

                // When it's night turn off solar irradiance sensor
		// turn on when its morning
		//if(packet.gps_hour == EVENING) packet.indicator &= ~APOGEE;
		//else if(packet.gps_hour == MORNING) packet.indicator |= APOGEE;

                transmitPacketBinary(); 
                clear_packet();
                sample_counter = 0;  // Clear the sample counter
            }

#else  // transmits using UART packet

            samplePacketUART();
            transmitPacketUART();

#endif

}

/***************************************************
 *  Name:        configureADC
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: configures the ADC. Normally, in the arduino 
                    IDE, we don't have to worry about this, but 
                    we change the registers to speed up the
                    ADC sample times a little. More docuementation
                    available online.
 ***************************************************/
void configureADC(void){

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

}

/***************************************************
 *  Name:        configurePins
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: Configures the default pin states to make sure
                    there isn't any strange behavior or 
                    unneeded power draw
 ***************************************************/
void configurePins(void){
    #ifdef WIND_SENSOR
    pinMode(_PIN_ANEMOMETER0, INPUT);
    pinMode(_PIN_ANEMOMETER1, OUTPUT);
    pinMode(_PIN_ANEMOMETER2, OUTPUT);
    pinMode(_PIN_ANEMOMETER3, OUTPUT);
    #endif

    #ifndef WIND_SENSOR
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

/***************************************************
 *  Name:        transmitPacketHello
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: Samples and transmits an initial packet
                    burst to help us test connectivity
 ***************************************************/
void transmitPacketHello(void){
    if(_CONFIG_PacketFormat == PACKET_BIN)
    {
        samplePacketBinary();
        samplePacketBinary();
        samplePacketBinary();
        samplePacketBinary();
        samplePacketBinary();
        transmitPacketBinary();
    }

}

/***************************************************
 *  Name:        sampleBatteryVoltage
 *  Returns:     an averaged battery voltage.
 *  Parameters:  None.
 *  Description: Samples the battery ADC voltage multiple times
                    and averages it 
 ***************************************************/
long sampleBatteryVoltage(void){
    double temp;
    for(i = 0; i < ADC_SAMPLE_NUM ; i++) {
        temp += analogRead(_PIN_BATT_V); 
    }
    temp = temp/ADC_SAMPLE_NUM;
    return ((temp*5000.0/1023));
}



/***************************************************
 *  Name:        sampleBatteryVoltageRaw
 *  Returns:     an averaged battery voltage.
 *  Parameters:  None.
 *  Description: like sampleBatteryVoltage() but returns a raw ADC 
 *                  value instead. May be useful later for saving
 *                  calculations
 ***************************************************/
double sampleBatteryVoltageRaw(void){
    double temp;
    for(i = 0; i < ADC_SAMPLE_NUM ; i++) {
        temp += analogRead(_PIN_BATT_V); 
    }
    temp = temp/ADC_SAMPLE_NUM;
    return temp;
}

    






