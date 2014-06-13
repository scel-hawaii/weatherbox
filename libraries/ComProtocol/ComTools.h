#include <inttypes.h>
#include  <XBee.h>

#define _SOME_HEADER 0xAA
#define _WP_HEADER 0xBB


typedef struct {
    
    uint8_t header;
    uint16_t nextPk;
    uint16_t totalPk;
} expPk;



void initExpPk(expPk* val);

void storeData(uint8_t *data, uint8_t length);

void requestPacket(XBee& xxbee, XBeeAddress64& xbeeAddr, expPk val);

void handleXBeeRx(ZBRxResponse& rx, expPk* val);