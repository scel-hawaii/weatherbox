
void getPacketHealth(void)
{
    health.schema = 4;
    health.address = address;
    health.uptime_ms = millis();
    health.batt_mv = analogRead(_PIN_BATT_V)*5/1023;
}

void transmitPacketHealth(void)
{
    memset(rf_payload, '\0', sizeof(rf_payload));
    memcpy(rf_payload, &health, sizeof(health));
    ZBTxRequest zbtx = ZBTxRequest(addr64, rf_payload, sizeof(health));
    xbee.send(zbtx);
}

void health_data_transmit(void)
{
    getPacketHealth();
    transmitPacketHealth();
}

void sendHealth(void)
{
    // Set health data to transmit every 10 minutes
    // Calculation: min*(60 sec/1 min)*(1000 milli-sec/1 sec)
    long transmit_health = 600000;

    if((millis() - health_transmit_timer) >= transmit_health)
    { 
            
        debug_msg("-------------------------------\n");
        debug_msg("TRANSMIT: Health Data\n");
        debug_msg("-------------------------------\n");

        // Power on system to transmit health data
        pstate_system(__ACTIVE);

        // Wait for the system to fully turn on
        transmit_timer = millis();
        int wake_time = 3000;
        while((millis() - transmit_timer) <= wake_time);

        // tranmsit health data
        health_data_transmit();

        // Power off system again until next health data transmission
        pstate_system(__POWER_SAVE);
        // Update time since last health transmission
        health_transmit_timer = millis();
    }
}

int chkHealth(void)
{
    int apogee_voltage = 0, panel_voltage = 0;

#ifdef HEALTH_GOOD_APOGEE
    apogee_voltage = analogRead(_PIN_APOGEE_V);
    if(apogee_voltage >= THRESH_GOOD_APOGEE_V)
#else //HEALTH_GOOD_PANEL
    panel_voltage = 2*analogRead(_PIN_SOLAR_V);
    if(panel_voltage >= THRESH_GOOD_PANEL_V)
#endif
        return GOOD_SOLAR;
    else if(LPF_get_current_output(&battery_filter) >= THRESH_GOOD_BATT_V)
        return NORMAL;
    else
        return POOR;
}
