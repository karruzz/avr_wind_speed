/*
 * Adc.c
 *
 * Created: 21.09.2014 11:24:47
 *  Author: karruzz
 * License: GNU GPL 3
 */
#include "Adc.h"

void AC_On()
{
	// вкл питание датчика	
	DDRB |= _BV(DDRB3);
	PORTB |= _BV(PORTB3);
	
	DDRD &= ~_BV(DDRD6);
	DDRD &= ~_BV(DDRD7);
	PORTD &= ~_BV(PORTD6);	
	PORTD &= ~_BV(PORTD7);
	
	ACSR &= ~_BV(ACD);	
	ACSR |= _BV(ACBG) | _BV(ACIS1) | _BV(ACIC);	
	
	TCCR1B |= _BV(CS12); // /256
	TIMSK1 |= _BV(ICIE1); // прерывание по переполнению Т1
	
	StrobeTime = 0xFFFF;
}

void AC_Off()
{
	TIMSK1 &= ~_BV(TOIE1);
	ACSR &= ~_BV(ACIC);	
	ACSR |= _BV(ACD);
	
	// выкл питание датчика
	DDRB |= _BV(DDRB3);
	PORTB &= ~_BV(PORTB3);	
}

// таймер сработал на событие с компаратора
ISR(TIMER1_CAPT_vect)
{
	cli();
	TCNT1 = 0;
	StrobeTime = ICR1;
	sei();
}
