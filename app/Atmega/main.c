#ifndef F_CPU					// if F_CPU was not defined in Project -> Properties
#define F_CPU 8000000UL			// define it now as 1 MHz unsigned long
#endif

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "leds.c"
#include "lcd.c"
#include "analog.c"
#include "slave.c"

void setRele(int to) {
	if (to) {
		PORTA |= 1 << 7;
		} else {
		PORTA &= ~(1 << 7);
	}
}

int main(void)
{
	DDRA = 0b11110000;
	DDRB = 0b11111111;
	DDRC = 0b11111111;
	DDRD = 0b11111111;
	PORTA = 0;
	PORTB = 0;
	PORTC = 1 << 5;
	PORTD = 0;

	MCUCSR = 1 << JTD;
	MCUCSR = 1 << JTD;

	startTimer();
	startLCD();
	startAnalog();
	startSlave();

	sei();

	while (1)
	{
		if (TIFR & 1 << TOV2) {
			TIFR |= 1 << TOV2;
			//setRele(!(PORTA & 1 << 7));
			LCDOverflow();
		}

		if (ADCSRA & 1 << ADIF) {
			ADCSRA |= 1 << ADIF;

			analogFlag();
		}

		if (SPSR & 1 << SPIF) {
			// falg is reset by reading SPDR

			slaveInterrupt();
		}
	}
}
