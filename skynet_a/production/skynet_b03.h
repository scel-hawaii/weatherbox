/****************************************************
 *
 *          skynet_b03.h
 *          REIS Weatherbox Firmware b03
 *          
 *          The main driver header file for the REIS weatherbox. 
 *
 ****************************************************/


long sampleBatteryVoltage(void);
long sampleSmoothBatteryV(int sample);

void configureWDT(void);
void enterSleep(void);

void configurePins(void);

void transmitPacketHello(void);
