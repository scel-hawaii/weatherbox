/* 

Our awesome skynet testbench program.. 

Keep this file mainly empty. We want to abstract everything 
we can outside of the arduino architecture, so when we move,
it'll be nice and easy. 

*/
#include <Adafruit_MCP4725.h>
#include <string.h>
#include <stdio.h>
#include <Wire.h>

#include "driver.h"
#include "cmd_parser.h"
#include "setup.h"
#include "dac.h"

Adafruit_MCP4725 dac2;

void setup(){
    Serial.println("INO: Starting setup routine");
    setup_routine();
    Serial.println("INO: Finished setup routine");
}

void loop(){
    Serial.println("INO: Starting main routine");
    main_routine();
    Serial.println("INO: WARNING: OUT OF MAIN ROUTINE!!");
}

