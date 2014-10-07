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

#define WIND_MEASURE 500
#define SIM900_LOAD 10000
#define WIND_MEASURE_PAUSE 2000

#include <stdio.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

const float K = -0.00526;
const float B = 29;
const long int N0 = 363;
long int SpeedAcc = 0;
long int SpeedOut = 0;
long int Speed = 0;

char s1[64];

int Ticks = 0;
int sign = 11;

int main(void)
{
	int isSend = 0;
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);
	Sim900PowerOn();
	_delay_ms(SIM900_LOAD);
	Sim900SendSpeed(0);
	Sim900PowerOff();	
			
	//wdt_enable(WDTO_8S);
    while(1)
    {
		AC_On();
		_delay_ms(WIND_MEASURE);		
		AC_Off();
		
		Speed = K*(StrobeTime - N0) + B;
		if (Speed < 0) Speed = 0;
		SpeedAcc += Speed - SpeedOut;
		SpeedOut = SpeedAcc >> 1;
		
		if (Ticks > 1800) // час
		{
			do 
			{
				Sim900PowerOn();
				_delay_ms(SIM900_LOAD);
				isSend = Sim900SendSpeed(sign);
				_delay_ms(SIM900_LOAD);			
				Sim900PowerOff();
			} while (isSend == 0);
			
			Ticks = 0;
			
			if (sign < 35) sign = 55;
			else sign = 11;
		}
		else 
		{
			_delay_ms(WIND_MEASURE_PAUSE);
			Ticks += 2;			
		}
	}	
}