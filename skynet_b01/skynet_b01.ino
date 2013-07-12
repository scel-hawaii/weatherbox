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

unsigned int battery;
unsigned int i;

long sampleBatteryVoltage(void){
    double temp;
    // sample 100 times and average it
    for(i = 0; i < 100 ; i++) {
        temp += analogRead(0); 
    }
    temp = temp/100;
    return (long)((temp*5000.0/1024));
}

void setup()
{
    Serial.begin(9600);
    softserial.begin(9600);
    xbee.begin(softserial);
    bmp085.begin();
    ina219_Solar.begin();

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library

    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

    
    for( int k = 0 ; k < 15 ; k++) {
        smooth_batt_array[k] = sampleBatteryVoltage();
        Serial.print("Initial Sample: ");
        Serial.print(k);
        Serial.print(" ");
        Serial.println(smooth_batt_array[k]);
    }

    /*
    Serial.println("Initial Smooth Voltage");
    sampleSmoothBatteryV(sampleBatteryVoltage());
    */

    if(smooth_batt_mv < 3700){ // Shutdown
        isEnoughVoltage = FALSE;
    }
    else{
        isEnoughVoltage = TRUE;
    }

    time = millis();
}

long sampleSmoothBatteryV(int sample){
    // keep a moving average of 15 samples
    double temp;
    Serial.print("Smooth Batt Sample #: ");
    Serial.println(smooth_sample_index);
    smooth_batt_array[smooth_sample_index] = sample;
    Serial.print("Sample: ");
    Serial.println(sample);
    Serial.print("Post Smooth Batt Sample #: ");
    Serial.println(smooth_sample_index);
    for(i = 0 ; i < BATT_SAMPLE_SIZE ; i++)
        temp += smooth_batt_array[i];
    for( i = 0 ; i < 15 ; i++) {
        Serial.println(smooth_batt_array[i]);
    }
    Serial.print("Temp: ");
    Serial.println(temp/BATT_SAMPLE_SIZE);
    smooth_sample_index += 1 % 15;
    return (temp/BATT_SAMPLE_SIZE);
    // return sample;
}


void loop() {
    if(isEnoughVoltage){
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
        smooth_batt_mv = sampleSmoothBatteryV(sampleBatteryVoltage());
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
        while(millis() - time < 1000);
        time = millis();

        // check if there is enough voltage
        if(smooth_batt_mv < 3700){
            // Shutdown
            isEnoughVoltage = FALSE;
        }

    }
    else {
        while(millis() - time < 1000);
        time = millis();
        sampleSmoothBatteryV(sampleBatteryVoltage());
        if(smooth_batt_mv > 3750)
            isEnoughVoltage = TRUE;
        else{ 
            isEnoughVoltage = FALSE;
        }
    }
    
}
