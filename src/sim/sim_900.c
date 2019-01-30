/*
 * Sim_900.c
 *
 * Created: 21.09.2014 10:15:38
 *  Author: karruzz
 * License: GNU GPL 3
 */

#define RESPONSE_WAIT 1500
#define DATA_WAIT 4000
#define RESET_WAIT 2000

#include "sim_900.h"

const char ECHO_OFF[] PROGMEM = "ATE0\n";

const char VERSION[] PROGMEM = "AT+CGMR\n";

const char CREG[] PROGMEM = "AT+CREG?\n";
const char CREG_RESPONSE[] PROGMEM = "+CREG: 0,1";

const char SAPBR_CONTYPE[] PROGMEM = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n";
const char SAPBR_APN[] PROGMEM = "AT+SAPBR=3,1,\"APN\",\"internet.mts.ru\"\n";
const char SAPBR_USER[] PROGMEM = "AT+SAPBR=3,1,\"USER\",\"mts\"\n";
const char SAPBR_PWD[] PROGMEM = "AT+SAPBR=3,1,\"PWD\",\"mts\"\n";
const char SAPBR_CONNECT[] PROGMEM = "AT+SAPBR=1,1\n";

const char HTTPINIT[] PROGMEM = "AT+HTTPINIT\n";
const char HTTPPARA_CID[] PROGMEM = "AT+HTTPPARA=\"CID\",1\n";
const char HTTPPARA_URL[] PROGMEM = "AT+HTTPPARA=\"URL\",\"http://www.blbx.ru/wind-add\"\n";
const char HTTPCONTENT[] PROGMEM = "AT+HTTPPARA=\"CONTENT\",\"application/json\"\n";
const char DOWNLOAD[] PROGMEM = "DOWNLOAD";
const char HTTPACTION[] PROGMEM = "AT+HTTPACTION=1\n"; // post
const char HTTPACTION_RESPONSE[] PROGMEM = "HTTPACTION:1,200,2";
const char HTTPREAD[] PROGMEM = "AT+HTTPREAD\n";
const char HTTPTERM[] PROGMEM = "AT+HTTPTERM\n";

const char CPOWD[] PROGMEM = "AT+CPOWD=1\n";

const char OK[] PROGMEM = "OK";

const char POWER_DOWN[] PROGMEM = "NORMAL POWER DOWN";


static char input[32];

static
int check_response(const char *response, int show)
{
	while (1) {
		memset(input, 0, sizeof(input));
		if (fgets(input, sizeof(input) - 1, stdin) == 0) return -1;
		if (memmem_P(input, strlen(input), response, strlen_P(response))) {
#ifdef DEBUG
			if (show > 0) fprintf(stdout, "<+ %s", input);
#endif
			return 0;
		}
#ifdef DEBUG
		if (show > 0) fprintf(stdout, "<- %s", input);
#endif
		_delay_ms(300);
	}
}

static
int send_command_with_check(const char *command, const char *response, int attempts)
{
	int attempt = 0;
	while(attempt < attempts) {
		clearerr(stdin);
		fprintf_P(stdout, command);
		_delay_ms(RESPONSE_WAIT);
		if (check_response(response, 1) == 0) return 0;
		attempt++;
	}
#ifdef DEBUG
	fprintf(stdout, "failed\n");
#endif
	return -1;
}

#define CHECK(cmd, resp, attempt) \
	if (send_command_with_check(cmd, resp, attempt) < 0) break

static char veloc_s[128];
void sim900_send_speeds(unsigned int speeds[], int count)
{
	do {
		CHECK(ECHO_OFF, OK, 3);
		CHECK(CREG, CREG_RESPONSE, 3);

		CHECK(SAPBR_CONTYPE, OK, 1);
		CHECK(SAPBR_APN, OK, 1);
		CHECK(SAPBR_USER, OK, 1);
		CHECK(SAPBR_PWD, OK, 1);
		CHECK(SAPBR_CONNECT, OK, 5);

		// http request
		CHECK(HTTPINIT, OK, 3);
		CHECK(HTTPPARA_CID, OK, 3);
		CHECK(HTTPPARA_URL, OK, 1);
		CHECK(HTTPCONTENT, OK, 1);

		// prepare and json
		char *s = veloc_s;
		s += sprintf(s, "{ \"velocity\" : [ ");
		for (int i = 0; i < count - 1; ++i)
			s += sprintf(s, "%u, ", speeds[i]);
		sprintf(s, "%u ] }\n", speeds[count - 1]);

		clearerr(stdin);
		fprintf(stdout, "AT+HTTPDATA=%u,%u\n", strlen(veloc_s), DATA_WAIT - 1000);
		_delay_ms(RESPONSE_WAIT);
		if (check_response(DOWNLOAD, 0) != 0) break;

		clearerr(stdin);
		fprintf(stdout, "%s\n", veloc_s);
		_delay_ms(DATA_WAIT);
		if (check_response(OK, 1) != 0) break;

		CHECK(HTTPACTION, OK, 1);
#ifdef DEBUG
		CHECK(HTTPREAD, HTTPACTION_RESPONSE, 3);
#endif
		CHECK(HTTPTERM, OK, 1);
		return;
	} while(0);

#ifdef DEBUG
	fprintf(stdout, "failed\n");
#endif
}

void sim900_on()
{
	// reset pin up-down
	PORTD |= _BV(PORTD3);
	_delay_ms(RESET_WAIT);
	PORTD &= ~_BV(PORTD3);
}

void sim900_off()
{
	send_command_with_check(CPOWD, POWER_DOWN, 5);
}
