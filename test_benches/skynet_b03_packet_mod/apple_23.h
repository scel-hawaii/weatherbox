// pins!
#define _PIN_ANEMOMETER0 A0    // IR_output
#define _PIN_ANEMOMETER1 10    // IR_enable
#define _PIN_ANEMOMETER2 11    // IR_transistor_select 1
#define _PIN_ANEMOMETER3 12    // IR_transistor_select 2

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

// Packet indicator bit placement
#define ADDRESS      0x1 
#define UPTIME       0x2
#define N            0x4
#define BATTERY      0x8
#define SOLAR_PANEL  0x10
#define BMP085_PRESS 0x20
#define BMP085_TEMP  0x40
#define HUMIDITY     0x80
#define APOGEE       0x100
#define GPS_VAL_SATS 0x200
#define DATE         0x400
#define TIME         0x800
#define COORDS       0x1000
#define GPS_ELEV     0x2000

// Define start byte for recieve code
#define _start_byte 126
#define NEW_FORMAT 255

// Define Morning time and evening time for solar irradiance
#define MORNING 16
#define EVENING 4

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

#define THRESH_GOOD_BATT_V 3750 // in milli volts
// configuration macros
#define ADC_SAMPLE_NUM 200 // number of samples taken for averaging 

#define PACKET_UART 0
#define PACKET_BIN 1

#define PACKET_BINARY   // comment out if using UART packet
//#define WIND_SENSOR     // comment out if no wind sensor

const int _CONFIG_PacketFormat = PACKET_BIN; // determines which packet is used
const int _CONFIG_TransmitPeriod = 60; // determines how often packet transmits

#define pMode_NORMAL 0
#define pMode_POWERSAVE 1
#define POWERSAVE

