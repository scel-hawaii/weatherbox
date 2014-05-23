typedef struct {

    uint8_t high;
    uint8_t low;
    uint16_t final;
} eepromAddr;



typedef struct {
    
    uint8_t header;
    uint16_t nextPk;
    uint16_t totalPk;
} expPk;
