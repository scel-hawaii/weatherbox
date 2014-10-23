#include "ADC.h"
// Define various ADC prescaler
const unsigned char PS_16 = (1 << ADPS2);
const unsigned char PS_32 = (1 << ADPS2) | (1 << ADPS0);
const unsigned char PS_64 = (1 << ADPS2) | (1 << ADPS1);
const unsigned char PS_128 = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

/***************************************************
 *  Name:        configureADC
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: configures the ADC. Normally, in the arduino 
                    IDE, we don't have to worry about this, but 
                    we change the registers to speed up the
                    ADC sample times a little. More docuementation
                    available online.
 ***************************************************/
void configureADC(void){

    // Setup faster ADC 
    ADCSRA &= ~PS_128;  // remove bits set by Arduino library
    // you can choose a prescaler from above.
    // PS_16, PS_32, PS_64 or PS_128
    ADCSRA |= PS_64;    // set our own prescaler to 64 

}
