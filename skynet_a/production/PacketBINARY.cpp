/****************************************************
 *
 *          PacketBINARY.ino
 *
 ****************************************************/
#include "PacketBINARY.h"

/***************************************************
 *  Name:        clear_packet()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  This function zeros out the binary packet before transmission
 *
 ***************************************************/
void Packet_init(int address)
{
    packet.schema = 3;
    packet.address = address;
    packet.overflow_num = 0;
    packet.uptime_ms = 0;
    packet.n = 0;
    packet.bmp085_press_pa = 0;
    packet.bmp085_temp_decic = 0;
    packet.humidity_centi_pct = 0;
    int i;
    for (i = 0; i < 60; i++)
    {
        packet.batt_mv[i/10] = 0;
        packet.panel_mv[i/10] = 0;
        packet.apogee_w_m2[i/3] = 0;
    }
}
/***************************************************
 *  Name:        samplePacketBinary()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  This function samples, and stuffs the results into a binary packet.
 *  The binary packet format can take up to 60 samples, each subsequeunt sample 
 *  iterates the number of data points, allowing us to keep track of how many
 *  samples we've already taken. The sample function takes care of the stacking. 
 * 
 *  Solar irradance is sampled each second, and the other less important data 
 *  points can are sampled less frequently.
 *
 ***************************************************/
void Packet_samplePacketBinary(void)
{
        int n = packet.n;

        // Checking if millis() has overflowed
        // If the previous uptime is greater than the current millis()
        // an overflow has occurred.
        unsigned long uptime = millis();
        packet.overflow_num += chk_overflow(uptime, packet.uptime_ms);
        packet.uptime_ms = uptime;

	    // initialize values
        int i; float batt_mv_raw = 0, panel_mv_raw = 0, apogee_raw = 0;

        batt_mv_raw = Sensors_sampleBatterymV();
        panel_mv_raw = Sensors_samplePanelmV();
        apogee_raw = Sensors_sampleSolarIrrmV();

	    // Save values to packet
        packet.batt_mv[n/10] = batt_mv_raw;
        packet.panel_mv[n/10] = panel_mv_raw;
        packet.bmp085_press_pa = Sensors_samplePressurepa();
        packet.bmp085_temp_decic = Sensors_sampleTempdecic();
        packet.humidity_centi_pct = Sensors_sampleHumiditypct();
        packet.apogee_w_m2[n/3] = apogee_raw;
        packet.n += 1;
}

void Packet_transmitPacketBinary(void){
}
