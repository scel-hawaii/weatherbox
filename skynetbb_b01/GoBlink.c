/*
 * main.c
 *
 * Created: 10/14/2013 7:35:39 PM
 *  Author: Andy Pham
 */ 

#include <avr/io.h>
#define F_CPU 10000000UL
#include <util/delay.h>

int main(void)
{
	DDRD |= (1 << PIND7);
	PORTD &= ~(1 << PIND7);
	while(1) {
		PORTD ^= (1 << PIND7);
		_delay_ms(100);
	}
}

