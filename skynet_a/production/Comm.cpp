// Comm.cpp

#include "Comm.h"

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
void Comm_initXbee(void){
    xbee = XBee();
    addr64 = XBeeAddress64(0, 0);

    #ifdef TESTBENCH_DEBUG 
        xbee.begin(softserial);
    #else
        xbee.begin(Serial);
    #endif
}

void Comm_transmitPacketXbe(uint8_t* rf_payload) {
    debug_msg("\n------------------------------\n");
    debug_msg("Transmit Packet\n");
    debug_msg("------------------------------\n");
    ZBTxRequest zbTx = ZBTxRequest(addr64, rf_payload, sizeof(rf_payload));
    xbee.send(zbTx);
}




