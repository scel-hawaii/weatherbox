**********************************************
*       Weatherbox Version: skynet_b03
**********************************************

Major version changes:
----------------------------------------------
* Restructured program and more modularized code
* Getting ready for on the fly config changes
    via UART.
* Dual binary and UART packet capability 


Major Changelog (See git log for minor notes):
----------------------------------------------
8/23/13 - Sean Hara

BETTER better test update.

8/22/13 - Zachary Dorman

Better test update.

8/22/13 - Sean Hara

Test Update.

8/16/13 - Kenny Luong

Found out that the binary packet isn't comptible with the 
xbee in END POINT mode. I suspect that this may be because
of the xbee's sleep mode interfering with our packet 
being split up by the xbee's firmware. More research to be 
done later. 


**********************************************
*       Weatherbox Version: skynet_b01
**********************************************

Major version changes:
----------------------------------------------
* Re-worked previous "alpha version" code
* Consolidated changes into one file, and released major version

Major Changelog (See git log for minor notes):
----------------------------------------------
UNKNOWN DATE - Kenny Luong

Found out that the arduino has some strange behavior with the  
delay functions. The arduino uses atmel's compiler, which apparently
doesn't work all the time. Previous code sent UART packets out 
in uneven intervals, and not all the time. Reading from the 
millis() timer and using a delay appeared to fix the issue.
