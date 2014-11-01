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

const char ECHO_OFF[] PROGMEM = "ATE0";

const char CREG[] PROGMEM = "AT+CREG?";
const char CREG_RESPONSE[] PROGMEM = "+CREG: 0,1";

const char SAPBR_CONTYPE[] PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"";
const char SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"";
const char SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",\"mts\"";
const char SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",\"mts\"";
const char SAPBR_CONNECT[] PROGMEM = "AT+SAPBR=1,1";

const char HTTPINIT[] PROGMEM = "AT+HTTPINIT";
const char HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1";
const char HTTPPARA_URL[] PROGMEM = "AT+HTTPPARA=\"URL\",\"http://www.kt815.ru/AddSpeed.aspx?speed=";
const char HTTPACTION[] PROGMEM = "AT+HTTPACTION=0"; // get запрос
const char HTTPACTION_RESPONSE[] PROGMEM = "HTTPACTION:0,200";
const char HTTPREAD[] PROGMEM = "AT+HTTPREAD";

const char CPOWD[] PROGMEM = "AT+CPOWD=1";

const char OK[] PROGMEM = "OK";

const char POWER_DOWN[] PROGMEM = "NORMAL POWER DOWN";


char s[30];
char url[64];

int CheckResponse(const char *response)
{
	memset(s, 0, sizeof(s));
	while (fgets(s, sizeof(s)-1, stdin))
	{		
		fprintf(stdout, "<%s>\n", s);
		if (memmem_P(s, strlen(s), response, strlen_P(response))) 
		{
			_delay_ms(300);
			return 1;
		}
		_delay_ms(300);		
		memset(s, 0, sizeof(s));		
	}
	return 0;	
}

void SendCommandWithCheck(const char *command, const char *response, int attempts)
{
	int attempt = 0;
	while(attempt < attempts) {				
		clearerr(stdin);
		fprintf_P(stdout, command);
		fprintf(stdout, "\n");
		_delay_ms(RESPONSE_WAIT);
		if (CheckResponse(response)) break;
		attempt++;
	}
}

void Sim900SendSpeed(unsigned int speeds[], int count)
{	
	SendCommandWithCheck(ECHO_OFF, OK, 5);
	
	SendCommandWithCheck(CREG, CREG_RESPONSE, 5);

	SendCommandWithCheck(SAPBR_CONTYPE, OK, 1);
	SendCommandWithCheck(SAPBR_APN, OK, 1);
	SendCommandWithCheck(SAPBR_USER, OK, 1);
	SendCommandWithCheck(SAPBR_PWD, OK, 1);
	
	SendCommandWithCheck(SAPBR_CONNECT, OK, 5);
	
	// http request
	SendCommandWithCheck(HTTPINIT, OK, 1);
	SendCommandWithCheck(HTTPPARA_CID, OK, 1);
	
	int attempt = 0;
	while(attempt < 5)
	{
		clearerr(stdin);
		fprintf_P(stdout, HTTPPARA_URL);
		_delay_ms(500);
		for (int i = 0; i < count - 1; i++)
		{
			fprintf(stdout, "%u_", speeds[i]);
			_delay_ms(200);
		}
		fprintf(stdout, "%u\"\n", speeds[count - 1]);
		_delay_ms(RESPONSE_WAIT);
		if (CheckResponse(OK)) break;
		attempt++;	
	}
	
	SendCommandWithCheck(HTTPACTION, HTTPACTION_RESPONSE, 5);
}

void Sim900PowerOn() 
{
	PORTD |= _BV(PORTD3);
	_delay_ms(2000);
	PORTD &= ~_BV(PORTD3);		
}

void Sim900PowerOff()
{
	SendCommandWithCheck(CPOWD, POWER_DOWN, 5);
}
