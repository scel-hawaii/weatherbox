/****************************************************
 *
 *          schema.h
 *
 *          This file contains the structure for our packet schema. The 
 *          Packet schema keeps all of the variables for the functions 
 *          which we'll send to the Xbee API to get transmitted. 
 *          
 *          Note: when a new packet schema is made, create a new 
 *          structure.
 *          
 *
 ****************************************************/
typedef struct {

    uint16_t schema;
    uint16_t address;
    uint32_t uptime_ms;
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[15];
    uint16_t panel_mv[15];
    uint32_t bmp085_press_pa;
    int16_t bmp085_temp_decic;
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
    uint16_t address;
    uint32_t uptime_ms;
    uint8_t n;			// number of data points in packet 0..30
    uint16_t batt_mv[15];
    uint16_t panel_mv[15];
    uint32_t bmp085_press_pa;
    int16_t bmp085_temp_decic;
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
