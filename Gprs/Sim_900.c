/*
 * Sim_900.c
 *
 * Created: 21.09.2014 10:15:38
 *  Author: karruzz
 * License: GNU GPL 3
 */

#define RESPONSE_WAIT 4000
#define RESPONSE_PAUSE 1000

#include "Sim_900.h"

const char CREG[] PROGMEM = "AT+CREG?\r\n";
const char CREG_RESPONSE[] PROGMEM = "+CREG: 0,1";

const char SAPBR_CONTYPE[] PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n";
const char SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"\n";
const char SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",\"mts\"\n";
const char SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",\"mts\"\n";
const char SAPBR_CONNECT[] PROGMEM = "AT+SAPBR=1,1\n";

const char HTTPINIT[] PROGMEM = "AT+HTTPINIT\n";
const char HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1\n";
const char HTTPPARA_URL[] = "AT+HTTPPARA=\"URL\",\"http://www.kt815.ru/AddSpeed.aspx?speed=";
const char HTTPACTION[] PROGMEM = "AT+HTTPACTION=0\n"; // get запрос
const char HTTPACTION_RESPONSE[] PROGMEM = "HTTPACTION:0,200";
const char HTTPREAD[] PROGMEM = "AT+HTTPREAD\n";

const char CPOWD[] PROGMEM = "AT+CPOWD=1\n";

const char OK[] PROGMEM = "OK";

char s[32];
char url[128];

int SendCommandWithCheck(int (*fp)(FILE *__stream, const char *__fmt, ...),
 const char *command, const char *response, unsigned int attempts)
{
	unsigned int attempt = 0;

	do
	{
		clearerr(stdin); // с прошлого раза
		fp(stdout, command);
		_delay_ms(RESPONSE_WAIT);
		memset(s, 0, sizeof(s));		
		while (fgets(s, sizeof(s) - 1, stdin))
		{
			if (memmem_P(s, strlen(s), response, strlen_P(response)))
			{
				_delay_ms(RESPONSE_PAUSE);
				return 1;
			}
			memset(s, 0, sizeof(s));
		}
		attempt++;
	} while (attempt < attempts);	
	return 0;
}

void Sim900SendSpeed(unsigned int speeds[], int count)
{	
	SendCommandWithCheck(&fprintf_P, CREG, CREG_RESPONSE, 5);

	SendCommandWithCheck(&fprintf_P, SAPBR_CONTYPE, OK, 1);
	SendCommandWithCheck(&fprintf_P, SAPBR_APN, OK, 1);
	SendCommandWithCheck(&fprintf_P, SAPBR_USER, OK, 1);
	SendCommandWithCheck(&fprintf_P, SAPBR_PWD, OK, 1);
	
	SendCommandWithCheck(&fprintf_P, SAPBR_CONNECT, OK, 5);
	
	// http request
	SendCommandWithCheck(&fprintf_P, HTTPINIT, OK, 1);
	SendCommandWithCheck(&fprintf_P, HTTPPARA_CID, OK, 1);
	
	memset(url, 0, sizeof(url));
	strcat(url, HTTPPARA_URL);
	for (int i = 0; i < count - 1; i++)	
	{
		char temp[6];
		sprintf(temp, "%u_", speeds[i]);
		strcat(url, temp);		
	}
	char temp[7];
	sprintf(temp, "%u\"\n", speeds[count - 1]);	
	strcat(url, temp);
	SendCommandWithCheck(&fprintf, url, OK, 1);	
	SendCommandWithCheck(&fprintf_P, HTTPACTION, HTTPACTION_RESPONSE, 5);
}

void Sim900PowerOn() 
{
	PORTD |= _BV(PORTD5);
	_delay_ms(2000);
	PORTD &= ~_BV(PORTD5);		
}

void Sim900PowerOff()
{
	SendCommandWithCheck(&fprintf_P, CPOWD, OK, 5);
}
