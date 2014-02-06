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
    packet.schema = 4;
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
	packet.batt_mv[i/10] = 0;
	packet.panel_mv[i/10] = 0;
	packet.apogee_w_m2[i] = 0;
#ifdef WIND_SENSOR
        packet.wind_ir_off_1[i/10] = 0;
        packet.wind_ir_off_2[i/10] = 0;
        packet.wind_ir_off_b[i/10] = 0;
        packet.wind_ir_on_1[i/10] = 0;
        packet.wind_ir_on_2[i/10] = 0;
        packet.wind_ir_on_b[i/10] = 0;
#endif
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
        int i; 
	float batt_mv_raw = 0, panel_mv_raw = 0, apogee_raw = 0;

        // Find the averages of these values 
        for (i = 0; i < 10; i += 1)
        {
            batt_mv_raw += analogRead(_PIN_BATT_V)*5000.0/1023;
            panel_mv_raw += 2*analogRead(_PIN_SOLAR_V)*5000.0/1023;
            apogee_raw += analogRead(_PIN_APOGEE_V)*5000.0/1023;
        }

	// Save values to packet
	uint8_t battnum = n/(60/samp.battery);
	uint8_t pannum = n/(60/samp.solar_panel);
	uint8_t apognum = n/(60/samp.apogee);
        packet.batt_mv[battnum] = batt_mv_raw/10.0;
        packet.panel_mv[pannum] = panel_mv_raw/10.0;
        packet.bmp085_press_pa = bmp085.readPressure();
        packet.bmp085_temp_decic = bmp085.readTemperature()*10;
        packet.apogee_w_m2[apognum] = apogee_raw/10.0;

#ifdef WIND_SENSOR
	sampleIRreading();    // gets the IR voltages
#endif

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
    int sizePacket;
    // Empty for now
    memset(rf_payload, '\0', sizeof(rf_payload));
    sizePacket = getPacket();
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizePacket);
    xbee.send(zbTx);
}

/***************************************************
 *  Name:        getPacket()
 *  Returns:     size of packet
 *  Parameters:  None.
 *  Description: This function stores the desired 
 *  packet values into the payload for sending.
 *
 ***************************************************/
int getPacket()
{
    int i = 0;
    int count = 0;
	
    // packet indicator
    rf_payload[count] = packet.indicator >> 8;
    count++;
    rf_payload[count] = packet.indicator & 0xFF;
    count++;

    // packet schema
    rf_payload[count] = packet.schema >> 8;
    count++;
    rf_payload[count] = packet.schema & 0xFF;
    count++;

    if(packet.indicator & ADDRESS)
    {
        rf_payload[count] = packet.address >> 8;
	count++;
	rf_payload[count] = packet.address & 0xFF;
	count++;
    }
    if(packet.indicator & UPTIME)
    {
        rf_payload[count] = packet.uptime_ms >> 24;
	count++;
	rf_payload[count] = (packet.uptime_ms >> 16) & 0xFF;
	count++;
	rf_payload[count] = (packet.uptime_ms >> 8) & 0xFF;
	count++;
	rf_payload[count] = packet.uptime_ms & 0xFF;
	count++;
    }
    if(packet.indicator & N)
    {
        rf_payload[count] = packet.n;
	count++;
    }
    if(packet.indicator & BATTERY)
    {
        for(i = 0; i < samp.battery; i++)
	{
	    rf_payload[count] = packet.batt_mv[i] >> 8;
	    count++;
	    rf_payload[count] = packet.batt_mv[i] & 0xFF;
	    count++;
	}
    }
    if(packet.indicator & SOLAR_PANEL)
    {
        for(i = 0; i < samp.solar_panel; i++)
	{
	    rf_payload[count] = packet.panel_mv[i] >> 8;
	    count++;
	    rf_payload[count] = packet.panel_mv[i] & 0xFF;
	    count++;
	}
    }
    if(packet.indicator & BMP085_PRESS)
    {
        rf_payload[count] = packet.bmp085_press_pa >> 24;
	count++;
        rf_payload[count] = (packet.bmp085_press_pa >> 16) & 0xFF;
	count++;
        rf_payload[count] = (packet.bmp085_press_pa >> 8) & 0xFF;
	count++;
        rf_payload[count] = packet.bmp085_press_pa & 0xFF;
	count++;
    }
    if(packet.indicator & BMP085_TEMP)
    {
        rf_payload[count] = packet.bmp085_temp_decic >> 8;
	count++;
	rf_payload[count] = packet.bmp085_temp_decic & 0xFF;
	count++;
    }
    if(packet.indicator & HUMIDITY)
    {
        rf_payload[count] = packet.humidity_centi_pct >> 8;
	count++;
        rf_payload[count] = packet.humidity_centi_pct & 0xFF;
	count++;
    }
    if(packet.indicator & APOGEE)
    {
        for(i = 0; i < samp.apogee; i++)
	{
	    rf_payload[count] = packet.apogee_w_m2[i] >> 8;
	    count++;
	    rf_payload[count] = packet.apogee_w_m2[i] & 0xFF;
	    count++;
	}
    }
    if(packet.indicator & GPS_VAL_SATS)
    {
        rf_payload[count] = packet.gps_valid_sats;
	count++;
    }
    if(packet.indicator & DATE)
    {
    	// gps year
        rf_payload[count] = packet.gps_year;
	count++;
    	// gps month
        rf_payload[count] = packet.gps_month;
	count++;
    	// gps day
        rf_payload[count] = packet.gps_day;
	count++;
    }
    if(packet.indicator & TIME)
    {
    	// gps hour
        rf_payload[count] = packet.gps_hour;
	count++;
    	// gps minute
        rf_payload[count] = packet.gps_minute;
	count++;
    	// gps second
        rf_payload[count] = packet.gps_second;
	count++;
    }
    if(packet.indicator & COORDS)
    {
    	// gps latitude
        rf_payload[count] = packet.gps_lat >> 24;
	count++;
        rf_payload[count] = packet.gps_lat >> 16 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_lat >> 8 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_lat & 0xFF;
	count++;

	// gps longitude
        rf_payload[count] = packet.gps_lon >> 24;
	count++;
        rf_payload[count] = packet.gps_lon >> 16 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_lon >> 8 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_lon & 0xFF;
	count++;
    }
    if(packet.indicator & GPS_ELEV)
    {
        rf_payload[count] = packet.gps_elev >> 24;
	count++;
        rf_payload[count] = packet.gps_elev >> 16 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_elev >> 8 & 0xFF;
	count++;
        rf_payload[count] = packet.gps_elev & 0xFF;
	count++;
    }
    return count;
}

/***************************************************
 *  Name:        sampleIRreading()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *
 *  Takes sample voltages from the IR sensor, averages over 100 samples
 *  and stores the data in the packet data structure.
 ***************************************************/
#ifdef WIND_SENSOR

void sampleIRreading()
{
    int n = packet.n;
    int i;

    float ir_transistor_1 = 0, ir_transistor_2 = 0, ir_transistor_b = 0;

    // IR not enabled and transistors 1 and 2 are off
    digitalWrite(_PIN_ANEMOMETER1,LOW);
    digitalWrite(_PIN_ANEMOMETER3,LOW);
    digitalWrite(_PIN_ANEMOMETER2,LOW);

    delay(1); // Wait for fall time of IR emitters (takes 20ns)

    for(i = 0; i < 100; i += 1)
    {
       // transistor 1 on and transistor 2 off
       digitalWrite(_PIN_ANEMOMETER2,HIGH);
       ir_transistor_1 += analogRead(_PIN_ANEMOMETER0)*5000/1023;
       
       // transistor 1 on and transistor 2 on
       digitalWrite(_PIN_ANEMOMETER3,HIGH);
       ir_transistor_b += analogRead(_PIN_ANEMOMETER0)*5000/1023;

       // transistor 1 off and transistor 2 on
       digitalWrite(_PIN_ANEMOMETER2,LOW);
       ir_transistor_2 += analogRead(_PIN_ANEMOMETER0)*5000/1023;
    }

    // Stores average sample value for off state
    packet.wind_ir_off_1[n/30] = ir_transistor_1/100;
    packet.wind_ir_off_2[n/30] = ir_transistor_2/100;
    packet.wind_ir_off_b[n/30] = ir_transistor_b/100;

    // clear values
    ir_transistor_1 = 0, ir_transistor_2 = 0, ir_transistor_b = 0;

    // IR enabled and transistors 1 and 2 are off
    digitalWrite(_PIN_ANEMOMETER1,HIGH);
    digitalWrite(_PIN_ANEMOMETER3,LOW);
    
    delay(1); // Wait for rise time of IR emitters (takes 20ns)

    for(i = 0; i < 100; i += 1)
    {
       // transistor 1 on and transistor 2 off
       digitalWrite(_PIN_ANEMOMETER2,HIGH);
       ir_transistor_1 += analogRead(_PIN_ANEMOMETER0)*5000/1023;

       // transistor 1 on and transistor 2 on
       digitalWrite(_PIN_ANEMOMETER3,HIGH);
       ir_transistor_b += analogRead(_PIN_ANEMOMETER0)*5000/1023;

       // transistor 1 off and transistor 2 on
       digitalWrite(_PIN_ANEMOMETER2,LOW);
       ir_transistor_2 += analogRead(_PIN_ANEMOMETER0)*5000/1023;

    }

    // Stores average sample value for on state
    packet.wind_ir_on_1[n/30] = ir_transistor_1/100;
    packet.wind_ir_on_2[n/30] = ir_transistor_2/100;
    packet.wind_ir_on_b[n/30] = ir_transistor_b/100;
}

#endif







