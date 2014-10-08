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

#define WIND_MEASURE_MS 120
#define SPEEDS_BLOCK_LENGTH 20
#define HARDWARE_LOAD_MS 10000

#include <stdio.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

// 1400-0.8 370-2.8
const float K = -0.0194;
const float B = 28;
const long int N0 = 365;
long int Speed = 0;
long int StrobeAcc;
long int StrobeOut;
long int Ticks = 0;

unsigned int speeds[SPEEDS_BLOCK_LENGTH];
char s1[64];

int main(void)
{
	int isSend = 0;
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);	
	fprintf(stdout, "start\n");

    while(1)
    {
		AC_On();
		for (int i = 0; i < SPEEDS_BLOCK_LENGTH; i++)
		{
			Ticks = 0;
			while (Ticks < WIND_MEASURE_MS)
			{
				_delay_ms(1000);
				StrobeAcc += StrobeTime - StrobeOut;
				StrobeOut = StrobeAcc >> 8;				
				Ticks++;	
			}
			
			Speed = K * (StrobeOut - N0) + B;
			if (Speed < 0) Speed = 0;
			speeds[i] = (unsigned int)Speed;
		}
		AC_Off();		
		
		do 
		{
			Sim900PowerOn();
			_delay_ms(HARDWARE_LOAD_MS);
			isSend = Sim900SendSpeed(speeds, SPEEDS_BLOCK_LENGTH);
			Sim900PowerOff();
			_delay_ms(2000);
		} while (isSend == 0);		
	}	
}