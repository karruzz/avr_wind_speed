/*
 * Adc.c
 *
 * Created: 21.09.2014 11:24:47
 *  Author: karruzz
 * License: GNU GPL 3
 */
#include "Adc.h"

unsigned short _strobes;

void AC_Init(void)
{

	StrobesCount = 10;
}

// инициализация ацп
void ADC_Init(void)
{
	// инициализируем ADC
	ADMUX = _BV(REFS0) |	// источник опорного напряжения - AVCC
	_BV(ADLAR) |	// левосторонний формат преобразований
	_BV(MUX0);	// неинвертирующий ADC0, инвертирующий ADC1 усиление 1
	
	ADCSRA = _BV(ADEN) |	// Разрешение работы ADC
	_BV(ADATE) |	// ADC в режиме автоматического перезапуска
	_BV(ADIE) |	// разрешается прерывание по завершении преобразования ADC
	_BV(ADSC) | 	// Запуск преобразования ADC
	2;			// делитель 8
}

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
	ACSR |= _BV(ACIE) | _BV(ACBG) | _BV(ACIS1);	
	
	TCCR1B |= _BV(CS12); // /256
	TIMSK1 |= _BV(TOIE1); // прерывание по переполнению Т1
	
	TCCR0B |= _BV();
	StrobesCount = 0;
	_strobes = 0;
}

void AC_Off()
{
	TIMSK1 &= ~_BV(TOIE1);
	ACSR &= ~_BV(ACIE);	
	ACSR |= _BV(ACD);
	
	// выкл питание датчика
	DDRB |= _BV(DDRB3);
	PORTB &= ~_BV(PORTB3);	
	
	StrobesCount = 0;
}

ISR(ANALOG_COMP_vect)
{
	cli();
	_strobes++;	
	sei();		
}

// таймер дошел до конца - не было импульсов, значит частота 0
ISR(TIMER1_OVF_vect)
{
	cli();
	StrobesCount = _strobes;
	_strobes = 0;
	sei();
}