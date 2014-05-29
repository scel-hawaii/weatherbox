/****************************************************
 *
 *          schema.h
 *
 *          This file contains the structure for our packet schema. The 
 *          Packet schema keeps all of the variables for the functions 
 *          which we'll send to the Xbee API to get transmitted. 
 *          
 *          Note: when a new packet schema is made, create a new 
 *          structure. BINARY packet is position dependent on where
 *	    everything is placed and sent.
 *          
 *
 ****************************************************/
typedef struct {

    uint16_t schema;
    uint16_t address;		// Address of Arduino	
    uint32_t uptime_ms;		// Time since start of program
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[15];	// Battery Voltage (in milli volts)
    uint16_t panel_mv[15];	// Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;  // Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[60];

    uint8_t gps_valid_sats;
    uint8_t gps_year;
    uint8_t gps_month;
    uint8_t gps_day;
    uint8_t gps_hour;
    uint8_t gps_minute;
    uint8_t gps_second;
    int32_t gps_lat;
    int32_t gps_lon;
    int32_t gps_elev;
} schema_1;

typedef struct {

    uint16_t schema;
    uint16_t address;		// Address of Arduino
    uint32_t uptime_ms;		// Time since start of program
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[15];	// Battery Voltage (in milli volts)
    uint16_t panel_mv[15];	// Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;	// Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[60];

    uint8_t gps_valid_sats;
    uint8_t gps_year;
    uint8_t gps_month;
    uint8_t gps_day;
    uint8_t gps_hour;
    uint8_t gps_minute;
    uint8_t gps_second;
    int32_t gps_lat;
    int32_t gps_lon;
    int32_t gps_elev;

    int16_t dallas_amb_c[15];
    int16_t dallas_roof_c[15];
    int16_t panel_ua[15];

} schema_2;

typedef struct {

    uint16_t schema;
    uint16_t address;		    // Address of Arduino	
    uint8_t overflow_num;   // Number of times millis overflowed (happens ~every 49 days)
    uint32_t uptime_ms;		    // Time since start of program
    uint8_t n;			        // number of data points in packet 0..30
    uint16_t batt_mv[6];	    // Battery Voltage (in milli volts)
    uint16_t panel_mv[6];	    // Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;  // Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[20];
} schema_3;

typedef struct {
    uint16_t schema;
    uint16_t address;
    uint8_t overflow_num;   // Number of times millis overflowed (happens ~every 49 days)
    uint32_t uptime_ms;
    uint16_t batt_mv;
} schema_5;

typedef schema_5 schema_health;

typedef struct {
    uint16_t schema;
    char text[70];
} schema_6;

typedef schema_6 schema_debug;
