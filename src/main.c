/*
 * Gprs.c
 *
 * Created: 20.09.2014 18:18:42
 *  Author: karruzz
 * License: GNU GPL 3
 */

#define F_CPU 3686400UL
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1

#define WIND_MEASURE_S 120
#define SPEEDS_BLOCK_LENGTH 15
#define FACTORS 3
#define WAIT_CONNECTION 10000

#include <stdio.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

unsigned int speeds[SPEEDS_BLOCK_LENGTH];

float K[FACTORS] = { -0.00866, -0.00583, -0.003 };
int x0[FACTORS] = { 1200, 2400, 10000 };
int B[FACTORS] = { 33, 26, 4 };
	
unsigned int Speed(unsigned int x)
{
	long int xTemp = x;
	for (int i = 0; i < FACTORS; i++)
	{
		if (xTemp < x0[i]) return (unsigned int) (K[i] * (xTemp - x0[i]) + B[i]);
	}	
	return 0;	
}	

int main(void)
{	
	int Ticks;
	unsigned int SpeedSum;
		
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);		
	
	wdt_disable();	
	
	Sim900PowerOn();
	_delay_ms(5000);
	Sim900PowerOff();
			
    while(1)
    {
	    AC_On();
	    wdt_enable(WDTO_8S);
	    for (int i = 0; i < SPEEDS_BLOCK_LENGTH; i++)
	    {
		    Ticks = 0;
		    SpeedSum = 0;
		    while (Ticks < WIND_MEASURE_S)
		    {
			    _delay_ms(995);
			    SpeedSum += Speed(StrobeTime);
			    Ticks++;
				wdt_reset();
		    }
		    
		    speeds[i] = (unsigned int)(SpeedSum / WIND_MEASURE_S);	
			fprintf(stdout, "%u ", speeds[i]);
	    }
		wdt_disable();
	    AC_Off();
	    fprintf(stdout, "\n");
		
	    Sim900PowerOn();
	    _delay_ms(WAIT_CONNECTION);
	    Sim900SendSpeed(speeds, SPEEDS_BLOCK_LENGTH);
	    Sim900PowerOff();
    }	
}