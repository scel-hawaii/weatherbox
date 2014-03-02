/* 

Our awesome skynet testbench program.. 

Keep this file mainly empty. We want to abstract everything 
we can outside of the arduino architecture, so when we move,
it'll be nice and easy. 

*/
#include <string.h>
#include <stdio.h>
#include "cmd_parser.h"
#include "driver.h"
#include "setup.h"

void setup(){
    setup_routine();
}

void loop(){
    main_routine();
}





