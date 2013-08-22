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

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(0, 0);

uint8_t payload[243];

// Grab the address from the Arduino EEPROM 
long address = EEPROM.read(2) | (EEPROM.read(3)<<8);

long batt_mv, panel_mv, panel_ua;
long bmp085_temp_decic;
long bmp085_press_pa;
long apogee_mv, apogee_w_m2;
long dallas_rooftemp_c, dallas_ambtemp_c;

unsigned int i;
unsigned int pMode;

String s; 

int len;

uint8_t rf_payload[243];
schema_1 packet;

long sample_counter = 0; 

/***************************************************
 *      setup()
 *
 *      This function is run once at the beginning of the start
 *      sequence, before the setup function. Put any init scripts 
 *      you may need in here. 
 ***************************************************/
void setup() {
    Serial.begin(9600);

    xbee.begin(Serial);
    bmp085.begin();
    ina219_Solar.begin();
    
    // Configuration Scripts
    configurePins();
    configureADC();

    clear_packet();

    delay(2000);
    transmitPacketHello();  // Say Hello

    configureWDT();
    digitalWrite(_PIN_PSWITCH, HIGH);
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
    // Turn on all the sensors and Xbee
    digitalWrite(_PIN_XBEE_SLEEP, LOW);

    // Check the watchdog timer flag 
    if(f_wdt == 1)
    {
        int batteryV = sampleBatteryVoltage();
        if(batteryV > THRESH_GOOD_BATT_V){
            digitalWrite(_PIN_PSWITCH, HIGH);
            sampleANDtransmit();
            delay(100);                             // Delay and wait for transmit to finish
        }
        else{
            while(batteryV < 3800) {
                digitalWrite(_PIN_PSWITCH, LOW);    
                batteryV = sampleBatteryVoltage();
                delay(100);                         // Wait 100 ms for everything to settle
            }
        }
        f_wdt = 0;
        enterSleep();
    }
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







