/****************************************************
 *
 *          PacketUART.ino
 *
 ****************************************************/


/***************************************************
 *  Name:        samplePacketUART()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  Samples for the packet using a string. The string is actually a JSON 
 *  packet format, and can be parsed later on the server side code. 
 *
 ***************************************************/
void samplePacketUART(void)
{
    /*
        s = "{";
        s += "\"address\": ";
        s += String(address);
        s += ", \"uptime_ms\": ";
        s += String(millis());
        s += ", \"bmp085_temp_decic\": ";
        bmp085_temp_decic = bmp085.readTemperature()*10;
        s += String(bmp085_temp_decic);
        s += ", \"bmp085_press_pa\": ";
        bmp085_press_pa = bmp085.readPressure();
        s += String(bmp085_press_pa);
        s += ", \"batt_mv\": ";
        batt_mv = sampleBatteryVoltage();
        softserial.println(batt_mv);
        s += String(batt_mv);

        s += ", \"panel_mv\": ";
        panel_mv = 2*analogRead(1)*5000.0/1024;
        s += String(panel_mv);
        s += ", \"apogee_mv\": ";
        apogee_mv = analogRead(2)*5000.0/1024;
        s += String(apogee_mv);
        s += ", \"apogee_w_m2\": ";
        apogee_w_m2 = apogee_mv*5.0;
        s += String(apogee_w_m2);

        s += ", \"panel_ua\": ";
        panel_ua = ina219_Solar.getCurrent_mA()*1000;
        s += String(panel_ua);

        s += ", \"dallas_roof_c\": ";
        dallas_roof_sen.requestTemperatures();
        dallas_rooftemp_c = dallas_roof_sen.getTempCByIndex(0);
        s += String(dallas_rooftemp_c);

        s += ", \"dallas_amb_c\": ";
        dallas_amb_sen.requestTemperatures();
        dallas_ambtemp_c = dallas_amb_sen.getTempCByIndex(0);
        s += String(dallas_ambtemp_c);
        s += "}";
        s += "       ";		// it explodes without something here... omg wtf

        // s += "blah blah blah";

        // s += "Hath not a Jew eyes? Hath not a Jew hands, organs, dimensions, senses, affections, passions, fed with the same food, hurt with the same weapons, subject to the same diseases, healed by the same means, warmed and cooled by the same winter and summer, as a Christian is? If you prick us, do we not bleed? If you tickle us, do we not laugh? If you poison us, do we not die? And if you wrong us, shall we not revenge?"; // s misbehaves when long
        */

}

/***************************************************
 *  Name:        transmitPacketUART()
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *  Transmits the string using the Xbee Arduino library. More info
 *  on the Xbee Arduino Library can be found online.
 *
 ***************************************************/
void transmitPacketUART()
{
    for (int i = 0; i < sizeof(payload); i++)
        payload[i] = i % 10 + '0';
    // payload[i] = '\0';

    // payload length init
    len = 0; 
    for (int i = 0; i < sizeof(payload) && s[i] != '\0'; len = ++i) // yikes
        payload[i] = s[i];

    ZBTxRequest zbTx = ZBTxRequest(addr64, payload, len);
    xbee.send(zbTx);
    softserial.println(s);
}
