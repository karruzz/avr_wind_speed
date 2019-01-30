/*
 * Gprs.c
 *
 * Created: 20.09.2014 18:18:42
 *  Author: karruzz
 * License: GNU GPL 3
 */

#include <stdio.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "uart.h"
#include "sim_900.h"
#include "adc.h"

// #define F_CPU 3686400UL  // in cmake
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1
#define SPEED_MEASURMENTS 15
#define FACTORS 3

const int WIND_MEASURE_S = 128; // 1 << 7
const int SIM_WAIT_CONNECTION = 8000;

FILE uart_stream = FDEV_SETUP_STREAM(uart_putc, uart_getc, _FDEV_SETUP_RW);

uint16_t speeds[SPEED_MEASURMENTS];

float K[FACTORS] = { -0.00866, -0.00583, -0.003 };
int16_t x0[FACTORS] = { 1200, 2400, 10000 };
int16_t B[FACTORS] = { 33, 26, 4 };

inline
unsigned int to_speed(unsigned int period)
{
	for (int i = 0; i < FACTORS; i++)
		if (period < x0[i])
			// always positive linear model
			return (uint16_t) K[i] * (period - x0[i]) + B[i];

	return 0;
}

int main(void)
{
	stdout = stdin = &uart_stream;
	uart_init(MYUBRR);

	wdt_disable();
#ifdef DEBUG
	fprintf(stdout, "debug\n");
#else
	fprintf(stdout, "no debug\n");
#endif

	// test on/off for visual control
	sim900_on();
	_delay_ms(SIM_WAIT_CONNECTION);
	sim900_off();

	while(1) {
		adc_on();
		wdt_enable(WDTO_8S);
		for (int i = 0; i < SPEED_MEASURMENTS; ++i) {
			int ticks = 0;
			uint16_t speeds_sum = 0;
			while (ticks < WIND_MEASURE_S) {
				_delay_ms(995);
				speeds_sum += to_speed(adc_get_strobe_period());
				ticks++;
				wdt_reset();
			}

			speeds[i] = (uint16_t)(speeds_sum >> 7);
		}
		wdt_disable();
		adc_off();

		sim900_on();
		_delay_ms(SIM_WAIT_CONNECTION);
		sim900_send_speeds(speeds, SPEED_MEASURMENTS);
		sim900_off();
	}
}
