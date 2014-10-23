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
    packet.schema = 1;
    packet.address = address;
    packet.uptime_ms = 0;
    packet.gps_valid_sats = 0;
    packet.n = 0;
    packet.bmp085_press_pa = 0;
    packet.bmp085_temp_decic = 0;
    packet.humidity_centi_pct = 0;
    int i;
    for (i = 0; i < 60; i += 1)
    {
	packet.batt_mv[i/4] = 0;
	packet.panel_mv[i/4] = 0;
	packet.apogee_w_m2[i] = 0;
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
    if (packet.n < 60)
    {
        int n = packet.n;
        packet.uptime_ms = millis();

	// initialize values
        int i; float batt_mv_raw = 0, panel_mv_raw = 0, apogee_raw = 0;

        // Find the averages of these values 
        for (i = 0; i < 10; i += 1)
        {
            batt_mv_raw += 7;
            panel_mv_raw += 7;
            apogee_raw += 7;
        }

	    // Save values to packet
        packet.batt_mv[n/4] = 7;
        packet.panel_mv[n/4] = 7;
        packet.bmp085_press_pa = 7;
        packet.bmp085_temp_decic = 7;
        packet.apogee_w_m2[n] = 7;
        packet.n += 1;
    }

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
    // Empty for now
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &packet, sizeof(packet));
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(packet));
    xbee.send(zbTx);
}
