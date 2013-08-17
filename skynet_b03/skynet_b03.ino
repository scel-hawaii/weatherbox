#include "skynet_b03.h"
#define BATT_SAMPLE_SIZE 15
// change me!!!11!1
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>

#include <EEPROM.h>
#include "apple_23.h"

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <SHT1x.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "schema.h"

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

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

unsigned int isEnoughVoltage = TRUE;

uint8_t payload[243];
long smooth_batt_array[15];
long smooth_batt_mv;
unsigned int smooth_sample_index = 0;

// Grab the address from the Arduino EEPROM 
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);
long batt_mv, panel_mv, panel_ua;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;
long dallas_rooftemp_c, dallas_ambtemp_c;

long time = 0;
volatile int f_wdt = 1;

unsigned int battery;
unsigned int i;
unsigned int pMode;

String s; 

int len;

uint8_t rf_payload[243];
schema_1 packet;

long sample_counter = 0; 

void setup() {
    Serial.begin(9600);
    xbee.begin(Serial);
    bmp085.begin();
    ina219_Solar.begin();
    configurePins();

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

    time = millis();
    clear_packet();

    delay(2000);
    transmitPacketHello();

    configureWDT();
}
void loop() {
    // Turn on all the sensors and Xbee
    digitalWrite(_PIN_XBEE_SLEEP, LOW);
    digitalWrite(_PIN_PSWITCH, HIGH);

    if(f_wdt == 1)
    {
        int batteryV = sampleBatteryVoltage();
        if(batteryV > THRESH_GOOD_BATT_V){
            sampleANDtransmit();
            delay(100);
        }
        else{
            while(batteryV < 3800) {
                digitalWrite(_PIN_PSWITCH, LOW);    
                batteryV = sampleBatteryVoltage();
                delay(100);
            }
        }
        f_wdt = 0;
        enterSleep();
    }
}

void sampleANDtransmit(void){
    switch(_CONFIG_PacketFormat){
        case PACKET_UART:
            samplePacketUART();
            transmitPacketUART();
            break;
        case PACKET_BIN:
            samplePacketBinary();
            sample_counter++;
            if(sample_counter == _CONFIG_TransmitPeriod) {
                transmitPacketBinary(); 
                clear_packet();
                sample_counter = 0;  // Clear the sample counter
            }
            break;
        default:
            break;
    }
}

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
 *  Name:        sampleSmoothBatteryV
 *  Returns:     smooth battery voltage.
 *  Parameters:  raw unsmooth sample.
 *  Description: Takes the current sample, puts it into
                    a running average, and spits back out a 
                    smooth sample. This is primarily used 
                    for voltage detection during the 
                    power cutoff point.
 ***************************************************/
long sampleSmoothBatteryV(int sample){
    double temp;
    /*
    smooth_batt_array[smooth_sample_index] = sample;

    // take the average of all the values
    for(i = 0 ; i < BATT_SAMPLE_SIZE ; i++)
        temp += smooth_batt_array[i];

    // Move the index up, and in a circle
    smooth_sample_index += 1 % 15;
    */

    //return (temp/BATT_SAMPLE_SIZE);
    return sample;
}



