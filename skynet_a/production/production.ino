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
#include "skynet.h"

#define FALSE 0
#define TRUE 1

#define __ON 1 
#define __OFF 0

#define __ACTIVE 1
#define __POWER_SAVE 0

#ifdef TESTBENCH_DEBUG
SoftwareSerial softserial(4, 5); //RX, TX
#else
SoftwareSerial softserial(4, 5); //RX, TX
#endif


/***************************************************
 *      setup()
 *
 *      This function is run once at the beginning of the start
 *      sequence, before the setup function. Put any init scripts 
 *      you may need in here. 
 ***************************************************/
void setup() {
    debug_text.schema = 6;
    // Set the communication speeds
    #ifdef TESTBENCH_DEBUG
    Serial.begin(115200);
    #else
    Serial.begin(9600);
    #endif

    debug_msg("Begin Setup!");
    sendDebugPacket("Begin program!");

    // Wait for everything to settle down
    delay(100);

    // Initialize our battery sample by averaging 200 samples
    // and then sending that to our low pass filter 
    // by making that it initial sample.
    for(i = 0; i < 200 ; i ++) {
        battery_sample += analogRead(_PIN_BATT_V);
	    solar_sample += analogRead(_PIN_APOGEE_V);
    }
    battery_sample = battery_sample / 200;
    solar_sample = solar_sample / 200;
    LPF_filter_init(&battery_filter, (float)battery_sample, BATT_LOWPASS_ALPHA);
    LPF_filter_init(&solar_filter, (float)solar_sample, BATT_LOWPASS_ALPHA);

    debug_msg("Begin program!\n");
    
    // Initalize the Xbee depending on which mode we're set to.
    // The TESTBENCH_DEBUG mode assumes we're using software
    // serial for the xbee

    softserial.begin(9600);

    Comm_initXbee();

    
    // Configuration Scripts
    configurePins();

    // Disabled by Kenny on 02/07/14
    // configureADC();

    // Initialize the packet!
    Packet_init();

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
	// as well as solar irradiance voltage
        LPF_update_filter(&battery_filter, analogRead(_PIN_BATT_V));
        LPF_update_filter(&solar_filter, analogRead(_PIN_APOGEE_V));

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
        debug_msg("Finished watching serial\n");

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

            LPF_filter_init(&battery_filter, battery_filter.output, BATT_LOWPASS_ALPHA);
            LPF_filter_init(&solar_filter, solar_filter.output, BATT_LOWPASS_ALPHA);
        }
    }
}

void watch_serial(){
    #ifdef DEBUG_WATCH
    if(softserial.available()){
        softserial.println("ENTER DEBUG MODE");
        while(softserial.read() != '\n');
        while(1){
            if(softserial.available()){
                char input = softserial.read();
                softserial.print("GOT A COMMAND: ");
                softserial.println(input);
                while(softserial.read() != '\n');
                if(input == 'E') {
                    break;
                }
                else{
                    run_command(input);
                }
            }
        }
    }
    #endif
}
void run_command(char command){
    switch(command){
        case 'T':
            Serial.println("FUN");
            break;
        case 'B':
            Serial.println("CMD: Transmitting binary packet");
            Packet_transmitPacketBinary();
            break;
        case 'H':
            Serial.println("CMD: Transmitting health packet");
            health_data_transmit();
            break;
        case 'D':
            Serial.println("CMD: Transmitting a debug packet");
            sendDebugPacket("Here I am debugging!");
            break;
        // Get the current attached Xbee's Address and transmit it
        case 'A':
            break;

        default:
            break;
    }
}

void barebones_routine(){
    // sample and then increment the sample counter
    Packet_samplePacketBinary();
	sample_counter++;

    debug_msg("Sample count: ");
    debug_int(sample_counter);
    debug_msg("\n");

	if(sample_counter >= 60) {
        debug_msg("Transmitting!\n");
        Packet_transmitPacketBinary(); 
	    Packet_init();
	    sample_counter = 0;
	}

    // If it hasn't passed the wait time yet we wait, 
    // and once it has passed, we reset that timer again.
    // This way, we sample exactly every second
    long wait_millis = 1000;
    while((millis() -  transmit_timer) <= wait_millis );
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
            Packet_samplePacketBinary();
            sample_counter++;
	        // Check if desired amount of samples for transmit have been taken
            if(sample_counter == _CONFIG_TransmitPeriod) {
                Packet_transmitPacketBinary(); 
                Packet_init();
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
        Packet_samplePacketBinary();
        Packet_samplePacketBinary();
        Packet_samplePacketBinary();
        Packet_samplePacketBinary();
        Packet_samplePacketBinary();
        Packet_transmitPacketBinary();
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


void sendDebugPacket(char *dtext){
    debug_text.schema = 6;
    strcpy(debug_text.text, dtext);
    transmitDebug();
}

void transmitDebug(void) {
    debug_text.schema = 6;
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &debug_text, sizeof(debug_text));
    ZBTxRequest zbtx = ZBTxRequest(addr64, rf_payload, sizeof(debug_text));
    xbee.send(zbtx);
}
