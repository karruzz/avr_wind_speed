/*
 * Uart.c
 *
 * Created: 20.09.2014 18:25:47
 *  Author: karruzz
 * License: GNU GPL 3
 */

#include "uart.h"

FIFO( 64 ) uart_tx_fifo;
FIFO( 64 ) uart_rx_fifo;

void uart_init(uint16_t ubrr)
{
	// baudrate
	UBRR0H = (uint8_t)(ubrr>>8);
	UBRR0L = (uint8_t) ubrr;
	// 8 bit data, 1 stop, no parity
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	// enable, receive, response, interruption
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}

// data received from USART
ISR(USART_RX_vect)
{
	uint8_t rxbyte = UDR0;
	if (!FIFO_IS_FULL(uart_rx_fifo))
		FIFO_PUSH(uart_rx_fifo, rxbyte);
}

// data register accepts new data
ISR(USART_UDRE_vect)
{
	if (FIFO_IS_EMPTY(uart_tx_fifo)) {
		UCSR0B &= ~_BV(UDRIE0); // disable this interruption
	} else {
		char txbyte = FIFO_FRONT(uart_tx_fifo);
		FIFO_POP(uart_tx_fifo);
		UDR0 = txbyte;
	}
}

int uart_putc(char c, FILE *file)
{
	int ret;
	if (c == '\n')
		uart_putc('\r', file);

	cli(); //запрещаем прерывания
	if( !FIFO_IS_FULL(uart_tx_fifo) ) {
		FIFO_PUSH(uart_tx_fifo, c);
		UCSR0B |= _BV(UDRIE0);
		ret = 0;
	} else {
		ret = -1;
	}
	sei();

	return ret;
}

int uart_getc(FILE* file)
{
	int ret;

	cli();
	if( !FIFO_IS_EMPTY(uart_rx_fifo) ) {
		ret = FIFO_FRONT(uart_rx_fifo);
		FIFO_POP(uart_rx_fifo);
	} else {
		ret = _FDEV_EOF; // no data
	}
	sei();

	return ret;
}















