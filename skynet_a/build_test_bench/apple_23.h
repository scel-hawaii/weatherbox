/* =========================================================
 *
 *
 *      Apple 2.3 Configuration 
 *
 *      This file sets the main hardware configuration options 
 *      for apple 2.3
 * =========================================================
 */

#ifndef APPLE_23_H
#define APPLE_23_H

#define APPLE_VERSION 23


// =========================================
// 
//  DEBUG CONFIGURATION FLAGS
//
//  Enable  the following modes below by UNCOMMENTING them.
// ==========================================


// This enables our testbench debug platform. The testbench debug platform is 
// used primarily for testing on the ground. 
// It does primarily two things:
//  * switches the xbee to softwareserial (12, 11) (TX, RX)
//  * switches our debug messages to the hardware serial
// 
// #define TESTBENCH_DEBUG

// This flag enables our debug messages.
//
// #define DEBUG

// This flag enables debug messages on the softwareserial port (12, 11)(TX,RX).
// Primarily useful for testing when the board is already in production.
// TODO: This will be the primary debug mode later on, since we can program 
// specifically using the avrisp mkii now. 
// =============================================================================
#define DEBUG_SOFT

//
// Configure normal operation (CURRENTLY NOT TESTED)
// 
// #define NORMAL_OPERATION

// =========================================
// 
//  PIN CONFIGURATION (Do not change!)
//
// ==========================================
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

// Determines whether we use the solar irradiance or
// the panel voltage to check if we can send health
// data while the battery voltage is low
// If using solar irradiance keep defined
// If using the panel voltage, comment out
#define HEALTH_GOOD_APOGEE

// State of Health Macros
#define NORMAL 0
#define GOOD_SOLAR 1
#define POOR 2

// Sets the good voltage threshold 
// Below this the box goes into a power saving mode 
// and waits for the voltage to come back above the 
// THRESH_REINIT_SYSTEM, 3.730 volts or approx 45% capacity remaining
// Calculation: 3.730/5 * 1023 = 763.158 ~ 763
#define THRESH_GOOD_BATT_V 763

// When the battery voltage is low but above ~30%
// and the solar power is good, the box transmits
// health data every 10 minutes
// Calculation: 3.703/5 * 1023 = 757.6338 ~ 757 
#define THRESH_LOW_BATT_V 757

// The solar is considered good if the solar panel
// voltage is around 6 volts
// Calculation: 6/2/5 * 1023 = 613.8 ~ 614
#define THRESH_GOOD_PANEL_V 614

// Solar is considered good if the solar irradiance
// voltage is around 2.5 volts
// Calculation: 2.5/5 * 1023 = 511.5 ~ 511
#define THRESH_GOOD_APOGEE_V 511

// THRESH_REINIT_SYSTEM defines the voltage where the system 
// reinitializes transmission and sensing. 
// Check for the 3.830 volts (approximately 68% capacity remaining)
// Calculation: (3.83/5) * 1023 == 783.618 ~ 783
#define THRESH_REINIT_SYSTEM 783

// configuration macros
#define ADC_SAMPLE_NUM 30 

// This defines the alpha value for the battery lowpass filter
#define BATT_LOWPASS_ALPHA 0.005

// This defines the alpha value for the solar lowpass filter
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

#endif
