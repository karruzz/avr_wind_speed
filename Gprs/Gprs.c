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
#define WAIT_CONNECTION 10000

#include <stdio.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

unsigned int speeds[SPEEDS_BLOCK_LENGTH];

int main(void)
{
	long int Ticks;
	long unsigned int StrobeSum;
		
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);		
	
	Sim900PowerOn();
	_delay_ms(5000);
	Sim900PowerOff();
	
    while(1)
    {
	    AC_On();
	    for (int i = 0; i < SPEEDS_BLOCK_LENGTH; i++)
	    {
		    Ticks = 0;
		    StrobeSum = 0;
		    while (Ticks < WIND_MEASURE_S)
		    {
			    _delay_ms(1000);
			    StrobeSum += StrobeTime;
			    Ticks++;
		    }
		    
		    speeds[i] = (unsigned int)(StrobeSum / WIND_MEASURE_S);	
			speeds[i] = (unsigned int)(65535 - i*3000);
			fprintf(stdout, "%u ", speeds[i]);
	    }
	    AC_Off();
	    fprintf(stdout, "\n");
		
	    Sim900PowerOn();
	    _delay_ms(WAIT_CONNECTION);
	    Sim900SendSpeed(speeds, SPEEDS_BLOCK_LENGTH);
	    Sim900PowerOff();
    }	
}