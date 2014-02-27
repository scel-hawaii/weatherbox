/* =========================================================
 *
 *
 *      Apple 2.3 Configuration 
 *
 *      This file sets the main hardware configuration options 
 *      for apple 2.3
 * =========================================================
 */
#define APPLE_VERSION 23

// Define different operation modes here
// #define TESTBENCH_DEBUG
// #define DEBUG
// #define DEBUG_SOFT
// #define NORMAL_OPERATION

// pins!
#define _PIN_ANEMOMETER0 A0
#define _PIN_ANEMOMETER1 10
#define _PIN_ANEMOMETER2 11
#define _PIN_ANEMOMETER3 12 

#define _PIN_RX 0
#define _PIN_TX 1

#define _PIN_XBEE_TX 0
#define _PIN_XBEE_RX 1
#define _PIN_XBEE_SLEEP 13

#define _PIN_AMB_TEMP 2
#define _PIN_ROOF_TEMP 3

#define _PIN_GPS_RX 4
#define _PIN_GPS_TX 5

#define _PIN_PSWITCH 6

#define _PIN_HUMID_CLK 7
#define _PIN_HUMID_DATA 8


#define _PIN_SOLAR_V A1 
#define _PIN_APOGEE_V A2
#define _PIN_BATT_V A3
#define _PIN_SDA A4
#define _PIN_SCL A5

// Sets the good voltage threshold 
// Below this the box goes into a power saving mode 
// and waits for the voltage to come back above the 
// THRESH_REINIT_SYSTEM, 3.730 volts or approx 45% capacity remaining
// Calculation: 3.730/5 * 1023 = 763.158 ~ 763
#define THRESH_GOOD_BATT_V 763

// THRESH_REINIT_SYSTEM defines the voltage where the system 
// reinitializes transmission and sensing. 
// Check for the 3.830 volts (approximately 68% capacity remaining)
// Calculation: (3.83/5) * 1023 == 783.618 ~ 783
#define THRESH_REINIT_SYSTEM 783

// configuration macros
#define ADC_SAMPLE_NUM 30 

// This defines the alpha value for the battery lowpass filter
#define BATT_LOWPASS_ALPHA 0.005


//#define PACKET_U
#define PACKET_BINARY

#define PACKET_UART 0
#define PACKET_BIN 1

const int _CONFIG_PacketFormat = PACKET_BIN; // determines which packet is used
const int _CONFIG_TransmitPeriod = 60; // determines how often packet transmits

#define pMode_NORMAL 0
#define pMode_POWERSAVE 1
#define POWERSAVE
