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
    uint16_t batt_mv[10]; // Battery Voltage (in milli volts)
    uint16_t panel_mv[10]; // Panel Voltage (in milli volts)
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
    uint16_t indicator;

    uint16_t schema;
    uint16_t address;		// Address of Arduino	
    uint32_t uptime_ms;		// Time since start of program
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[10]; // Battery Voltage (in milli volts)
    uint16_t panel_mv[10]; // Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;  // Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[60];	// Adjust size with caution.

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
} schema_4;

typedef struct {
    uint8_t battery;
    uint8_t solar_panel;
    uint8_t apogee;
} Sample;


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
    uint16_t indicator;

    uint16_t schema;
    uint16_t address;		// Address of Arduino	
    uint32_t uptime_ms;		// Time since start of program
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[6];	// Battery Voltage (in milli volts)
    uint16_t panel_mv[6];	// Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;  // Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[6];

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
    
    uint16_t wind_ir_off_1[2]; // Wind IR reading (off state,transistor 1 on)
    uint16_t wind_ir_off_2[2]; // Wind IR reading (off state,transistor 2 on)
    uint16_t wind_ir_off_b[2]; // Wind IR reading (off state,both transistor on)
    uint16_t wind_ir_on_1[2];  // Wind IR reading (on state,transistor 1 on)
    uint16_t wind_ir_on_2[2];  // Wind IR reading (on state,transistor 2 on)
    uint16_t wind_ir_on_b[2];  // Wind IR reading (on state,both transistor on)
} schema_3;





