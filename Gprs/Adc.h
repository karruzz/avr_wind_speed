/*
 * Adc.h
 *
 * Created: 21.09.2014 11:24:29
 *  Author: karruzz
 * License: GNU GPL 3
 */


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile long int StrobeTime;

void AC_On();
void AC_Off();

#endif /* ADC_H_ */