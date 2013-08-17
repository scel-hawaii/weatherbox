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


