typedef struct 
{
  
    uint16_t schema;
    uint16_t address;		// Address of Arduino	
    uint32_t uptime_ms;		// Time since start of program
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[7];	// Battery Voltage (in milli volts)
    uint16_t panel_mv[7];	// Panel Voltage (in milli volts)
    uint32_t bmp085_press_pa;	// Pressure Value (in pascals)
    int16_t bmp085_temp_decic;  // Temperature Value (in celsius)
    uint16_t humidity_centi_pct;
    uint16_t apogee_w_m2[7];

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
