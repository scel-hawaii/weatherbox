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

// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

#define THRESH_GOOD_BATT_V 3700 // in milli volts
// configuration macros
#define ADC_SAMPLE_NUM 200 // number of samples taken for averaging 

#define PACKET_UART 0
#define PACKET_BIN 1

const int _CONFIG_PacketFormat = PACKET_BIN; 
const int _CONFIG_TransmitPeriod = 60;

#define pMode_NORMAL 0
#define pMode_POWERSAVE 1
#define POWERSAVE
