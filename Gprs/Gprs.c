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

#define WIND_MEASURE_S 5
#define SPEEDS_BLOCK_LENGTH 15
#define WAIT_CONNECTION 10000

#include <stdio.h>
#include <math.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

const float K1 = 57.46;
const float K2 = 0.00046;
long unsigned int StrobeSum;
long int Ticks = 0;

unsigned int speeds[SPEEDS_BLOCK_LENGTH];
unsigned int firstSpeed[1];
char s1[64];

int main(void)
{
	int isSend = 0;
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);	
	fprintf(stdout, "start\n");		
	unsigned int strobeAverage;

	AC_On();
    while(1)
    {
	//    AC_On();
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
		    
		    strobeAverage = StrobeSum / WIND_MEASURE_S;
		    speeds[i] = (unsigned int)(K1 / exp(K2 * strobeAverage));
			fprintf(stdout,"%u %u %lu\n", speeds[i], strobeAverage, StrobeSum);
	    }
	//    AC_Off();
	    
	    //Sim900PowerOn();
	    //_delay_ms(WAIT_CONNECTION);
	    //Sim900SendSpeed(speeds, SPEEDS_BLOCK_LENGTH);
	    //Sim900PowerOff();
    }	
}