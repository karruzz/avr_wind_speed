/*
 * Adc.c
 *
 * Created: 21.09.2014 11:24:47
 *  Author: karruzz
 * License: GNU GPL 3
 */

#include "adc.h"

static const int WAIT_HARDWARE_INIT_MS = 5000;
static volatile uint16_t strobe_period;

void adc_on()
{
	strobe_period = 0xFFFF;
	// sensor supply put on
	DDRD |=  _BV(DDRD5);
	PORTD |= _BV(PORTD5);

	DDRD  &= ~ (_BV(DDRD6)  | _BV(DDRD7));
	PORTD &= ~ (_BV(PORTD6) | _BV(PORTD7));

	ACSR &= ~_BV(ACD);
	ACSR |= _BV(ACBG) // connect internal voltage referencs source to INT0
		 | _BV(ACIS1) // interrupt by change signal from 1 to 0 overflow
		 | _BV(ACIC); // connect comparactor to capturing timer1

	TCCR1B |= _BV(CS11)  | _BV(CS10);  // 256
	TIMSK1 |= _BV(ICIE1) | _BV(TOIE1); // interrupt by Т1 overflow

	_delay_ms(WAIT_HARDWARE_INIT_MS);
}

void adc_off()
{
	TIMSK1 &= ~ (_BV(TOIE1) | _BV(ICIE1));
	ACSR &= ~_BV(ACIC);
	ACSR |= _BV(ACD);

	// sensor supply put off
	DDRD |= _BV(DDRD5);
	PORTD &= ~_BV(PORTD5);
}

uint16_t adc_get_strobe_period()
{
	return strobe_period;
}

ISR(TIMER1_CAPT_vect)
{
	cli();
	TCNT1 = 0;
	strobe_period = ICR1;
	sei();
}

ISR(TIMER1_OVF_vect)
{
	cli();
	strobe_period = 0xFFFF;
	sei();
}
