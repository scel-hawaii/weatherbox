
void getPacketHealth(void)
{
    health.schema = 4;
    health.address = address;
    health.uptime_ms = millis();
    health.n = n;
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
