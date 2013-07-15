#define NODE_ADDRESS 103
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

#define dataPin 8
#define clockPin 7
#define ONE_WIRE_BUS 2

#define FALSE 0
#define TRUE 1

#define pMode_NORMAL 0
#define pMode_POWERSAVE 1
#define POWERSAVE

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
volatile int f_wdt=1;

unsigned int battery;
unsigned int i;
unsigned int pMode;

long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);
void configureWDT(void);
void enterSleep(void);
void configurePins(void);

void setup() {
    Serial.begin(9600);
    xbee.begin(Serial);
    bmp085.begin();
    ina219_Solar.begin();
    configurePins();
    configureWDT();

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

    // Take 15 inital sample readings for moving average
    for( int k = 0 ; k < 15 ; k++) {
        smooth_batt_array[k] = sampleBatteryVoltage();
        softserial.print(k);
        softserial.print(" ");
        softserial.println(smooth_batt_array[k]);
    }

    
    // Double check if battery conditions are okay
    smooth_batt_mv = sampleSmoothBatteryV(sampleBatteryVoltage());
    if(smooth_batt_mv < 3700) 
        isEnoughVoltage = FALSE;
    else 
        isEnoughVoltage = TRUE;
    

    time = millis();
}

void loop() {
    if(f_wdt == 1)
    { 
        // If there is enough voltage
        if(isEnoughVoltage == TRUE){
            digitalWrite(_PIN_XBEE_SLEEP, LOW);
            digitalWrite(_PIN_PSWITCH, HIGH);

            softserial.print("Enough voltage: ");
            softserial.println(smooth_batt_mv);

            smooth_batt_mv = sampleSmoothBatteryV(sampleBatteryVoltage());
            if(smooth_batt_mv < 3700) 
                isEnoughVoltage = FALSE;
            else 
                isEnoughVoltage = TRUE;

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
            softserial.println(smooth_batt_mv);
            s += String(smooth_batt_mv);

            s += ", \"panel_mv\": ";
            panel_mv = 2*analogRead(1)*5000.0/1024;
            s += String(panel_mv);
            s += ", \"apogee_mv\": ";
            apogee_mv = analogRead(2)*5000.0/1024;
            s += String(apogee_mv);
            s += ", \"apogee_w_m2\": ";
            apogee_w_m2 = apogee_mv*5.0;
            s += String(apogee_w_m2);

            s += ", \"panel_ua\": ";
            panel_ua = ina219_Solar.getCurrent_mA()*1000;
            s += String(panel_ua);

            s += ", \"dallas_roof_c\": ";
            dallas_roof_sen.requestTemperatures();
            dallas_rooftemp_c = dallas_roof_sen.getTempCByIndex(0);
            s += String(dallas_rooftemp_c);

            s += ", \"dallas_amb_c\": ";
            dallas_amb_sen.requestTemperatures();
            dallas_ambtemp_c = dallas_amb_sen.getTempCByIndex(0);
            s += String(dallas_ambtemp_c);
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

            #ifndef POWERSAVE
            analogWrite(13, 255);
            #endif
            ZBTxRequest zbTx = ZBTxRequest(addr64, payload, len);
            xbee.send(zbTx);
            softserial.println(s);
            #ifndef POWERSAVE
            analogWrite(13, 0);
            #endif
            // delay(14800);
        }
        else {
            digitalWrite(_PIN_XBEE_SLEEP, HIGH);
            digitalWrite(_PIN_PSWITCH, LOW);

            long pureBatt = sampleBatteryVoltage();
            long smoothBatt = sampleSmoothBatteryV(pureBatt);

            softserial.print("Not Enough Voltage: ");
            softserial.print("Pure - ");
            softserial.println(pureBatt);
            softserial.print("Smooth - ");
            softserial.println(smoothBatt);

            smooth_batt_mv = smoothBatt;

            if(smooth_batt_mv >= 3750) isEnoughVoltage = TRUE;
            else isEnoughVoltage = FALSE;
        }
        time = millis();
        while(millis() - time < 50);
        f_wdt = 0;
        enterSleep();
    }
    else
    {

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
    return ((temp*5000.0/1024));
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
    softserial.println("WDT Overrun!!!");
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
