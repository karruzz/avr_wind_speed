/*
 * Gprs.c
 *
 * Created: 20.09.2014 18:18:42
 *  Author: karruzz
 * License: GNU GPL 3
 */

#define F_CPU 7372800UL
#define BAUD 19200
#define MYUBRR F_CPU/16/BAUD-1

#include <stdio.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"

FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);

const unsigned int SpeedTo_ms = 312;
const unsigned int K = 2;
unsigned int SpeedAcc;
unsigned int SpeedOut;

char s1[64];

int main(void)
{
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);
	AC_On();
	_delay_ms(3000);
	fprintf(stdout, "\n gprsInit\n");
	
	wdt_enable(WDTO_8S);
    while(1)
    {
		_delay_ms(3000);		
		unsigned long int speed = StrobesCount * SpeedTo_ms / 100;
		SpeedAcc += speed - SpeedOut;
		SpeedOut = SpeedAcc / K;
		wdt_reset();
	}	
}