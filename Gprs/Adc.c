/*
 * Adc.c
 *
 * Created: 21.09.2014 11:24:47
 *  Author: karruzz
 * License: GNU GPL 3
 */
#include "Adc.h"

#define HARDWARE_LOAD_MS 5000

void AC_On()
{
	StrobeTime = 0xFFFF;
	// вкл питание датчика	
	DDRB |= _BV(DDRB3);
	PORTB |= _BV(PORTB3);
		
	DDRD &= ~ (_BV(DDRD6) | _BV(DDRD7));
	PORTD &= ~ (_BV(PORTD6) | _BV(PORTD7));	
	
	ACSR &= ~_BV(ACD);	
	ACSR |= _BV(ACBG)   // подключение к выходу INT0 внутреннего ИОН
		  | _BV(ACIS1)  // срабатывание таймера - переход с 1 на 0
		  | _BV(ACIC);	// подключение компаратора к схеме захвата таймера1
	
	TCCR1B |= _BV(CS11) | _BV(CS10); // /256
	TIMSK1 |= _BV(ICIE1) | _BV(TOIE1); // прерывание по переполнению Т1	
	
	_delay_ms(HARDWARE_LOAD_MS);
}

void AC_Off()
{
	TIMSK1 &= ~ (_BV(TOIE1) | _BV(ICIE1));
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

ISR(TIMER1_OVF_vect)
{
	cli();
	StrobeTime = 0xFFFF;
	sei();
}
