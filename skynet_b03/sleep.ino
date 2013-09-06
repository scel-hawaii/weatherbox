/**************************************************** *
 *          sleep.ino
 *          
 *          This file contains the watchdog interrupt service
 *          routine, as well as some other relevant sleep functions.
 *          The sleep modes allow us to save some power on the 
 *          microcontroller, since we aren't processing 100% of the time.
 *
 ****************************************************/

/***************************************************
 *  Name:        configureWDT
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: configured the Watch Dog timer for the
                    appropriate settings
 ***************************************************/
void configureWDT(void){
    /* Clear the reset flag. */
    MCUSR &= ~(1<<WDRF);

    /* In order to change WDE or the prescaler, we need to
     * set WDCE (This will allow updates for 4 clock cycles).
     */
    WDTCSR |= (1<<WDCE) | (1<<WDE);

    /* set new watchdog timeout prescaler value */
    WDTCSR = 1<<WDP2 | 1<<WDP1; /* 1.0 seconds */
    //WDTCSR = 1<<WDP3; /* 4.0 seconds */
    //WDTCSR = 1<<WDP3 | 1<<WDP0; /* 8.0 seconds */

    /* Enable the WD interrupt (note no reset). */
    WDTCSR |= _BV(WDIE);

}

/***************************************************
 *  Name:        ISR(WDT_vect)
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: Watchdog Interrupt Service. This
 *               is executed when watchdog timed out.
 ***************************************************/
ISR(WDT_vect)
{
    if(f_wdt == 0)
    {
        f_wdt=1;
    }
    else
    {
        softserial.println("WDT Overrun!!!");
    }
}

/***************************************************
 *  Name:        enterSleep
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: Enters the arduino into sleep mode.
 ***************************************************/
void enterSleep(void)
{
  /* EDIT: could also use SLEEP_MODE_PWR_DOWN for lowest power consumption. 
     Information on other sleep modes can be found on the arduino website.
  */
  
  // set_sleep_mode(SLEEP_MODE_PWR_SAVE);   
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);   
  sleep_enable();
  
  /* Now enter sleep mode. */
  sleep_mode();
  
  /* The program will continue from here after the WDT timeout*/
  sleep_disable(); /* First thing to do is disable sleep. */
  
  /* Re-enable the peripherals. */
  power_all_enable();
}
