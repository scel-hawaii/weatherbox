/**************************************************** *
 *          sleep.h
 *          
 *          This main header file for the sleep.ino file.
 *
 ****************************************************/

#ifndef SLEEP_H
#define SLEEP_H
#include <avr/sleep.h>
volatile int f_wdt = 1;
void configureWDT(void);
void enterSleep(void);
#endif
