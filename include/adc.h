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

void adc_on();
void adc_off();
uint16_t adc_get_strobe_period();

#endif /* ADC_H_ */
