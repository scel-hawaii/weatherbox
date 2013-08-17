**********************************************
*
*       Readme file for skynet_b03
*
**********************************************

Major version changes:
----------------------------------------------
* Restructured program and more modularized code
* Getting ready for on the fly config changes
    via UART.
* Dual binary and UART packet capability 


Major Changelog (See git log for minor notes):
----------------------------------------------
8/16/13 - Kenny Luong

Found out that the binary packet isn't comptible with the 
xbee in END POINT mode. I suspect that this may be because
of the xbee's sleep mode interfering with our packet 
being split up by the xbee's firmware. More research to be 
done later. 

