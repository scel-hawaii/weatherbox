#define NODE_ADDRESS 103
#define BATT_SAMPLE_SIZE 15
// change me!!!11!1
#include <Wire.h>
#include <Adafruit_BMP085.h>

#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>

#include <EEPROM.h>

#define FALSE 0
#define TRUE 1


#define pMode_NORMAL 0
#define pMode_POWERSAVE 1

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>


// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

Adafruit_BMP085 bmp085;
Adafruit_INA219 ina219_Solar;

SoftwareSerial softserial(2, 3);

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

unsigned int isEnoughVoltage = TRUE;

uint8_t payload[243];
long smooth_batt_array[15];
long smooth_batt_mv;
unsigned int smooth_sample_index = 0;

// Grab the address from the Arduino EEPROM 
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);
long batt_mv, panel_mv, panel_ma;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;
long time = 0;
volatile int f_wdt=1;

unsigned int battery;
unsigned int i;
unsigned int pMode;

long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);

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
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 *
 ***************************************************/
ISR(WDT_vect)
{
  if(f_wdt == 0)
  {
    f_wdt=1;
  }
  else
  {
    Serial.println("WDT Overrun!!!");
  }
}

/***************************************************
 *  Name:        enterSleep
 *
 *  Returns:     Nothing.
 *
 *  Parameters:  None.
 *
 *  Description: Enters the arduino into sleep mode.
 *
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


void setup() {
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin();
    ina219_Solar.begin();
    configureWDT();

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

    // Take 15 inital sample readings for moving average
    for( int k = 0 ; k < 15 ; k++) {
        smooth_batt_array[k] = sampleBatteryVoltage();
        Serial.print(k);
        Serial.print(" ");
        Serial.println(smooth_batt_array[k]);
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
            Serial.print("Enough voltage: ");
            Serial.println(smooth_batt_mv);

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
            Serial.println(smooth_batt_mv);
            s += String(smooth_batt_mv);

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
        }
        else {
            long pureBatt = sampleBatteryVoltage();
            long smoothBatt = sampleSmoothBatteryV(pureBatt);

            Serial.print("Not Enough Voltage: ");
            Serial.print("Pure - ");
            Serial.println(pureBatt);
            Serial.print("Smooth - ");
            Serial.println(smoothBatt);

            smooth_batt_mv = smoothBatt;

            if(smooth_batt_mv >= 3750) isEnoughVoltage = TRUE;
            else isEnoughVoltage = FALSE;
        }
        delay(50);
        //time = millis();
        f_wdt = 0;
        enterSleep();
    }
    else
    {

    }
    
}

// Samples the battery voltage 100 times and takes an average
// Returns the average of 100 samples
long sampleBatteryVoltage(void){
    double temp;
    // sample 100 times and average it
    for(i = 0; i < 200 ; i++) {
        temp += analogRead(0); 
    }
    temp = temp/200;
    return ((temp*5000.0/1024));
}

// puts the a battery sample into a moving average
// and spits out a "smooth" battery voltage value
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


