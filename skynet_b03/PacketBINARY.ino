/****************************************************
 *
 *          PacketBINARY.ino
 *
 ****************************************************/


/***************************************************
 *  Name:        clear_packet()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  This function zeros out the binary packet before transmission
 *
 ***************************************************/
void clear_packet(void)
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
void samplePacketBinary(void)
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

        batt_mv_raw = analogRead(_PIN_BATT_V)*5000.0/1023;
        panel_mv_raw = 2*analogRead(_PIN_SOLAR_V)*5000.0/1023;
        apogee_raw = analogRead(_PIN_APOGEE_V)*5000.0/1023;

	    // Save values to packet
        packet.batt_mv[n/10] = batt_mv_raw;
        packet.panel_mv[n/10] = panel_mv_raw;
        packet.bmp085_press_pa = bmp085.readPressure();
        packet.bmp085_temp_decic = bmp085.readTemperature()*10;
        packet.humidity_centi_pct = sht1x.readHumidity();
        packet.apogee_w_m2[n/3] = apogee_raw;
        packet.n += 1;

}
/***************************************************
 *  Name:        transmitPacketBinary()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  Takes our packet data structure and then uses the Xbee Arduino API
 *  to package and send that data to the xbee. More information on the 
 *  Xbee library for arduino can be found online.
 *
 ***************************************************/
void transmitPacketBinary()
{
    debug_msg("\n------------------------------\n");
    debug_msg("TRANSMIT: Binary Packet \n");
    debug_msg("------------------------------\n");
    // Empty for now
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
}
