/*
 * Sim_900.c
 *
 * Created: 21.09.2014 10:15:38
 *  Author: karruzz
 * License: GNU GPL 3
 */

#include "Sim_900.h"

void Sim900On() 
{
	PORTD |= _BV(PORTD3);
	_delay_ms(100);
	PORTD |= _BV(PORTD2);
	_delay_ms(100);
	PORTD &= ~_BV(PORTD2);		
	_delay_ms(300);	
}

void Sim900Off()
{
	PORTD &= ~_BV(PORTD3);
}