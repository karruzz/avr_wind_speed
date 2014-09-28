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
#include <util/delay.h>
#include <string.h>
#include "Uart.h"
#include "Sim_900.h"
#include "Adc.h"
#include <avr/pgmspace.h>

const char d_out[] PROGMEM = ">";
const char d_in[] PROGMEM = "<";

const char ipr[] PROGMEM = "AT+IPR?\r\n";
const char gmm_test[] PROGMEM = "AT+GMM=?\r\n";
const char gmm[] PROGMEM = "AT+GMM\r\n";
const char imei_test[] PROGMEM = "AT+GSN=?\r\n";
const char imei[] PROGMEM = "AT+GSN\r\n";
const char cops[] PROGMEM = "AT+COPS?\r\n";
const char creg[] PROGMEM = "AT+CREG?\r\n";
const char csq[] PROGMEM = "AT+CSQ\r\n";

const unsigned int K_ms = 302;
char s[64];


FILE uart_stream = FDEV_SETUP_STREAM(Uart_putc, Uart_getc, _FDEV_SETUP_RW);
//FILE lcd_stream = FDEV_SETUP_STREAM(LCD_WriteData, NULL, _FDEV_SETUP_WRITE);

void SendCommand(const char *command)
{
	LCD_Clear();
	LCD_Goto(0,0);
	fprintf_P(stderr, d_out);
	fprintf_P(stderr, command);
	fprintf_P(stdout, command);
	_delay_ms(100);
			
	LCD_Goto(0,1);
	fprintf_P(stderr, d_in);
	while (!feof(&uart_stream))
	{
		LCD_Goto(1,1);
		fgets(s, sizeof(s)-1, stdin);
		LCD_Goto(1,1);
		fprintf(stderr, "%s ", s);
		_delay_ms(150);
	}
	clearerr(&uart_stream);
}

int main(void)
{
	stdout = stdin = &uart_stream;
	//stderr = &lcd_stream;	
	//LCD_Init();
	Uart_init(MYUBRR);
	
	//Sim900On();		
	
	//SendCommand(ipr);
	//SendCommand(ipr);
	//SendCommand(cops);
	//SendCommand(creg);
	//SendCommand(csq);
//	*s="asafa";
    while(1)
    {
		fgets(s, sizeof(s)-1, stdin);
		_delay_ms(2200);
		
		if (*s != '\0')
		{
			fprintf(stdout, "%s", s);
			if (strcmp("ac_on\r\n", s) == 0) 
			{
				AC_On();
				fprintf(stdout, "On_OK");
			}
			if (strcmp("ac_off\r\n", s) == 0)
			{
				 AC_Off();
				 fprintf(stdout, "Off_OK");
			}
			
			*s='\0';
		}
		unsigned long int speed = StrobesCount * K_ms / 100;
		fprintf(stdout, "%i\r\n", speed);
	}
}