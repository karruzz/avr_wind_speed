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

const unsigned int K_ms = 302;

char s1[64];

int main(void)
{
	stdout = stdin = &uart_stream;	
	Uart_init(MYUBRR);
	_delay_ms(3000);
	fprintf(stdout, "\r\ngprsInit\r\n");
	Sim900Init();
//	wdt_enable(WDTO_8S);
    while(1)
    {
		fgets(s1, sizeof(s1)-1, stdin);
		_delay_ms(2200);
		
		if (*s1 != '\0')
		{
			fprintf(stdout, "%s; %i", s1, strlen(s1));
			if (strcmp("ac_on\r\n", s1) == 0) 
			{
				AC_On();
				fprintf(stdout, "On_OK");
			}
			if (strcmp("ac_off\r\n", s1) == 0)
			{
				 AC_Off();
				 fprintf(stdout, "Off_OK");
			}
			
			*s1='\0';
		}
		unsigned long int speed = StrobesCount * K_ms / 100;

	//	wdt_reset();
	}	
}