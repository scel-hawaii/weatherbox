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
#include "low_pass.h"

/* Can't move libraries into skynet_b03.h. For some reason the program will 
   not compile if that happens. */

// explain what libraries are for and where to find them
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>
#include <XBee.h>
#include <Adafruit_INA219.h>

#include <EEPROM.h>

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <SHT1x.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#include "schema.h"
#include "sleep.h"
#include "apple_23.h"

#define FALSE 0
#define TRUE 1

#define __ON 1 
#define __OFF 0

#define __ACTIVE 1
#define __POWER_SAVE 0

OneWire oneWire1(_PIN_AMB_TEMP);
DallasTemperature dallas_amb_sen(&oneWire1);

OneWire oneWire2(_PIN_ROOF_TEMP);
DallasTemperature dallas_roof_sen(&oneWire2);

SHT1x sht1x(_PIN_HUMID_DATA, _PIN_HUMID_CLK);

Adafruit_BMP085 bmp085;
Adafruit_INA219 ina219_Solar;

#ifdef TESTBENCH_DEBUG
SoftwareSerial softserial(12, 11); //RX, TX
#else
SoftwareSerial softserial(12, 11); //RX, TX
#endif

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
uint8_t rf_payload[243];

//schema_1 packet;
schema_3 packet;
schema_health health;

// count number of samples taken
long sample_counter = 0; 

// global timers
unsigned long transmit_timer = 0; 
unsigned long health_transmit_timer = 0;

struct P_STATE{
    int xbee;
    int sensor_array;
};

P_STATE power_state;

LowPassFilter battery_filter;
long battery_sample = 0; 



/***************************************************
 *      setup()
 *
 *      This function is run once at the beginning of the start
 *      sequence, before the setup function. Put any init scripts 
 *      you may need in here. 
 ***************************************************/


void setup() {
    // Set the communication speeds
    Serial.begin(9600);

    debug_msg("Begin Setup!");

    // Wait for everything to settle down
    delay(100);

    // Initialize our battery sample by averaging 200 samples
    // and then sending that to our low pass filter 
    // by making that it initial sample.
    for(i = 0; i < 200 ; i ++) {
        battery_sample += analogRead(_PIN_BATT_V);
    }
    battery_sample = battery_sample / 200;
    LPF_filter_init(&battery_filter, (float)battery_sample, BATT_LOWPASS_ALPHA);

    debug_msg("Begin program!\n");
    
    // Initalize the Xbee depending on which mode we're set to.
    // The TESTBENCH_DEBUG mode assumes we're using software
    // serial for the xbee

softserial.begin(9600);
#ifdef TESTBENCH_DEBUG 
    xbee.begin(softserial);
#else
    xbee.begin(Serial);
#endif

    bmp085.begin();
    ina219_Solar.begin();
    
    // Configuration Scripts
    configurePins();

    // Disabled by Kenny on 02/07/14
    // configureADC();

    // Initialize the packet!
    clear_packet();

    // turn the power on!
    pstate_system(__ACTIVE);

    debug_msg("Wait for configuration set..\n");
    delay(500);
    debug_msg("Finished with setup!\n");

    // Disabled by Kenny on 02/07/14
    // configureWDT();
    
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
    int batt_voltage = 0; 

    // Active mode is 1
    // Command mode is 0
    int active_mode = 1;

    while(1){
        debug_msg("Begin Loop\n");

        // Sean: Update current battery voltage
        LPF_update_filter(&battery_filter, analogRead(_PIN_BATT_V));

        debug_msg("Finished updating filter.\n");

        // If the battery is good, keep running our routine 
        // TODO: When the power scheme is re-initialized remove the 1 
        // from the if statement and include our check
        // Sean: Checking if the battery voltage in mV is good

        batt_voltage = LPF_get_current_output(&battery_filter);

        debug_msg("Our battery voltage is at: ");
        debug_int(batt_voltage);
        debug_msg("\n");

        watch_serial();

        if(chkHealth() == NORMAL || chkHealth() == GOOD_SOLAR)
        {
            debug_msg("Voltage is good!\n");

            // Run the barebones routine forever
            transmit_timer = millis();
            barebones_routine();
        }

        // Otherwise, do this
        else{
            debug_msg("Voltage is not good!\n");
            // Shut down the power, and wait for it to be good
            // Sean: Shutting down xbee and sensors
            pstate_system(__POWER_SAVE);

            // Sean: updating battery voltage
            LPF_update_filter(&battery_filter, analogRead(_PIN_BATT_V));

            // Initialize time since last health data transmission
            health_transmit_timer = millis();

            // Keep checking to see if the battery is okay, 
            // and is above the certain threshold. Keep in mind that we do need 
            // a certain amount of distance between the cutoff voltage and the 
            // re-initialization voltage. 
            // Sean: checking voltage
            while(LPF_get_current_output(&battery_filter) < THRESH_REINIT_SYSTEM)
            {
                // Send the health data every 10 minutes
                sendHealth();
                
                // Delay every couple of millis 
                // Sean: Update timer to current time
                debug_msg("Check if voltage is good anymore: ");
                debug_float(LPF_get_current_output(&battery_filter));
                debug_msg("\n");


                transmit_timer = millis();
                int delay = 200;
                while((millis() - transmit_timer) <= delay);
                LPF_update_filter(&battery_filter, analogRead(_PIN_BATT_V));
                watch_serial();
            }
            
            // If we break out of this loop, lets re-initalize all of our systems // to make sure that we're good. 
            // Sean: re-initializing
            pstate_system(__ACTIVE);

            // Delay 3 seconds to allow xbee to wake up. Minimum start up time
        // should be 1 second at most but 3 seconds to be safe
        transmit_timer = millis();
        int wake_time = 3000;
        while((millis() - transmit_timer) <= wake_time);

        LPF_filter_init(&battery_filter, battery_filter.output, 0.001);
        }
    }
}

void watch_serial(){
    if(Serial.available()){
        Serial.println("ENTER DEBUG MODE");
        while(Serial.read() != '\n');
        while(1){
            if(Serial.available()){
                char input = Serial.read();
                Serial.print("GOT A COMMAND: ");
                Serial.println(input);
                while(Serial.read() != '\n');
                if(input == 'E') {
                    break;
                }
                else{
                    run_command(input);
                }
            }
        }
    }
}

void run_command(char command){
    switch(command){
        case 'T':
            Serial.println("FUN");
            break;
        case 'B':
            Serial.println("CMD: Transmitting binary packet");
            transmitPacketBinary();
            break;
        case 'H':
            Serial.println("CMD: Transmitting health packet");
            health_data_transmit();
            break;
        default:
            break;
    }
}

void barebones_routine(){
    // sample and then increment the sample counter
    samplePacketBinary();
	sample_counter++;

    debug_msg("Sample count: ");
    debug_int(sample_counter);
    debug_msg("\n");

	if(sample_counter >= 60) {
        debug_msg("Transmitting!\n");
transmitPacketBinary(); 
	    clear_packet();
	    sample_counter = 0;
	}

    // If it hasn't passed the wait time yet we wait, 
    // and once it has passed, we reset that timer again.
    // This way, we sample exactly every second
    long wait_millis = 1000;
    while( (millis() -  transmit_timer) <= wait_millis );
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
    // use ifdef instead of switch
#ifdef PACKET_U:
            samplePacketUART();
            transmitPacketUART();
#endif
#ifdef PACKET_BINARY:
            samplePacketBinary();
            sample_counter++;
	        // Check if desired amount of samples for transmit have been taken
            if(sample_counter == _CONFIG_TransmitPeriod) {
                transmitPacketBinary(); 
                clear_packet();
                sample_counter = 0;  // Clear the sample counter
            }
#endif
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
        #ifndef TESTBENCH_DEBUG
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
        #else
        #endif
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

/********************************
 * 
 *  Power Managment Functions 
 *
 ******************************/
void pstate_system(int state){
    if(state == __ACTIVE){
        pstate_xbee(__ON);
        pstate_sensor_array(__ON);
    }
    else if(state == __POWER_SAVE){
        pstate_xbee(__OFF);
        pstate_sensor_array(__OFF);
    }

}
// Switches the sleep states for the xbee
void pstate_xbee(int state){
    power_state.xbee = state; 
    sync_pstate();
}

// Switches the power state for the sensor array using the power switch
void pstate_sensor_array(int state){
    power_state.sensor_array = state; 
    sync_pstate();
}

// Sync the power states (called at the end of every pstate function)
void sync_pstate(void){
    digitalWrite(_PIN_XBEE_SLEEP, !power_state.xbee);
    digitalWrite(_PIN_PSWITCH, power_state.sensor_array);
}

/**************************************************************
 * 
 *
 * Debug functions to make turning debugging on and off easier
 *
 *
 *************************************************************/
void debug_msg(char message[]){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_float(float message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_int(int message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}

void debug_double(double message){
    #ifdef DEBUG
        Serial.print(message);
    #endif

    #ifdef DEBUG_SOFT
        softserial.print(message);
    #endif
}
