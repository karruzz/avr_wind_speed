/*
 * Sim_900.c
 *
 * Created: 21.09.2014 10:15:38
 *  Author: karruzz
 * License: GNU GPL 3
 */

#define RESPONSE_DELAY 3000

#include "Sim_900.h"

const char CREG[] PROGMEM = "AT+CREG?\r\n";
const char CREG_RESPONSE[] PROGMEM = "+CREG: 0,1";

const char SAPBR_CONTYPE[] PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
const char SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"\r\n";
const char SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",\"mts\"\r\n";
const char SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",\"mts\"\r\n";
const char SAPBR_CONNECT[] PROGMEM = "AT+SAPBR=1,1\r\n";

const char HTTPINIT[] PROGMEM = "AT+HTTPINIT\r\n";
const char HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1\r\n";
const char HTTPPARA_URL[] = "AT+HTTPPARA=\"URL\",\"http://alex-exe.ru/links/links.html\"\r\n";
const char HTTPACTION[] PROGMEM = "AT+HTTPACTION=0\r\n"; // get запрос
const char HTTPREAD[] PROGMEM = "AT+HTTPREAD\r\n";

const char OK[] PROGMEM = "OK";

char s[32];

int SendCommandWithCheck(int (*fp)(FILE *__stream, const char *__fmt, ...),
 const char *command, const char *response, unsigned int attempts)
{
	unsigned int attempt = 0;

	do
	{
		clearerr(stdin); // с прошлого раза
		fp(stdout, command);
		_delay_ms(RESPONSE_DELAY);
		memset(s, 0, sizeof(s));
		while (fgets(s, sizeof(s) - 1, stdin))
		{
			if (memmem_P(s, strlen(s), response, strlen_P(response)))
			{
				return 1;
			}
			memset(s, 0, sizeof(s));
		}
		attempt++;
	} while (attempt < attempts);	
	return 0;
}

void Sim900Init()
{
	SendCommandWithCheck(&fprintf_P, CREG, CREG_RESPONSE, 1);

	SendCommandWithCheck(&fprintf_P, SAPBR_CONTYPE, OK, 1);
	SendCommandWithCheck(&fprintf_P, SAPBR_APN, OK, 1);	
	SendCommandWithCheck(&fprintf_P, SAPBR_USER, OK, 1);	
	SendCommandWithCheck(&fprintf_P, SAPBR_PWD, OK, 1);	
	SendCommandWithCheck(&fprintf_P, SAPBR_CONNECT, OK, 1);	
}

void Sim900SendTemp()
{
	SendCommandWithCheck(&fprintf_P, HTTPINIT, OK, 5);
	SendCommandWithCheck(&fprintf_P, HTTPPARA_CID, OK, 5);
	SendCommandWithCheck(&fprintf, HTTPPARA_URL, OK, 5);
	SendCommandWithCheck(&fprintf_P, HTTPACTION, OK, 5);
	SendCommandWithCheck(&fprintf_P, HTTPREAD, OK, 5);
}

void Sim900PowerOn() 
{
	PORTD |= _BV(PORTD3);
	_delay_ms(100);
	PORTD |= _BV(PORTD2);
	_delay_ms(100);
	PORTD &= ~_BV(PORTD2);		
	_delay_ms(300);	
}

void Sim900PowerOff()
{
	PORTD &= ~_BV(PORTD3);
}