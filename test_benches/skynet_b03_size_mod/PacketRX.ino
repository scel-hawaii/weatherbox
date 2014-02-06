/***************************************************
 *  Name:        configureINT
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: 
 ***************************************************/
void configureINT(void)
{
    cli();
    TCCR1A = 0;
    TCCR1B = 0;

    OCR1A = 156;
    TCCR1B |= (1 << WGM12);
    TCCR1B |= (1 << CS10);
    TCCR1B |= (1 << CS12);
    TIMSK1 |= (1 << OCIE1A);
    sei();
}

/***************************************************
 *  Name:        ISR(TIMER1_COMPA_vect)
 *  Returns:     Nothing.
 *  Parameters:  None.
 *  Description: 
 ***************************************************/
ISR(TIMER1_COMPA_vect)
{
    PacketRX();
}

/***************************************************
 *  Name:        PacketRX
 *  Returns:     nothing
 *  Parameters:  None.
 *  Description: 
 *  
 *
 ***************************************************/

ZBRxResponse ZBrx = ZBRxResponse();

void PacketRX()
{
  xbee.readPacket();
  if(xbee.getResponse().isAvailable())
  {
    if(xbee.getResponse().getApiId() == ZB_RX_RESPONSE)
      {
        xbee.getResponse().getZBRxResponse(ZBrx);
      }
  }

  if(ZBrx.getData(0) == 0xFA)
  {
      packet.indicator = 0;
      for(uint8_t i = 1; i < 3; i++) 
        packet.indicator = (packet.indicator<<8) + ZBrx.getData(i);
      if(ZBrx.getData(3) <= 10)
        samp.battery = ZBrx.getData(3);
      if(ZBrx.getData(4) <= 10)
        samp.solar_panel = ZBrx.getData(4);
      if(ZBrx.getData(5) <= 40)
        samp.apogee = ZBrx.getData(5);
  }
}

