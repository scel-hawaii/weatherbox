/**************************************************** *
 *          sleep.h
 *          
 *          This main header file for the sleep.ino file.
 *
 ****************************************************/
volatile int f_wdt = 1;
volatile int f_wdt2 = 1;

void configureWDT(void);
void enterSleep(void);
