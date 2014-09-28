/*
 * Uart.c
 *
 * Created: 20.09.2014 18:25:47
 *  Author: karruzz
 * License: GNU GPL 3
 */

#include "Uart.h"

FIFO(64) uart_tx_fifo;
FIFO(64) uart_rx_fifo;

void Uart_init(unsigned int ubrr) 
{
	//настройка скорости обмена
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	//8 бит данных, 1 стоп бит, без контроля четности
	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
	//разрешить прием, передачу данных и прерывание по приёму байта
	UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0);
}

//Обработчик прерывания по окончанию приёма байта
ISR(USART_RX_vect)
{
	unsigned char rxbyte = UDR0;
	if (!FIFO_IS_FULL(uart_rx_fifo)) {
		FIFO_PUSH(uart_rx_fifo, rxbyte);
	}
}

ISR(USART_UDRE_vect)
{
	if (FIFO_IS_EMPTY(uart_tx_fifo)) {
		//если данных в fifo больше нет то запрещаем это прерывание
		UCSR0B &= ~_BV(UDRIE0);
	}
	else {
		//иначе передаем следующий байт
		char txbyte = FIFO_FRONT(uart_tx_fifo);
		FIFO_POP(uart_tx_fifo);
		UDR0 = txbyte;
	}
}

int Uart_putc(char c, FILE *file)
{
	int ret;
	cli(); //запрещаем прерывания
	if( !FIFO_IS_FULL(uart_tx_fifo) ) {
		//если в буфере есть место, то добавляем туда байт
		FIFO_PUSH(uart_tx_fifo, c);
		//и разрешаем прерывание по освобождению передатчика
		UCSR0B |= _BV(UDRIE0);
		ret = 0;
	}
	else {
		ret = -1; //буфер переполнен
	}
	sei(); //разрешаем прерывания
	return ret;
}

int Uart_getc(FILE* file)
{
	int ret;
	cli(); //запрещаем прерывания
	if( !FIFO_IS_EMPTY(uart_rx_fifo) ) {
		//если в буфере есть данные, то извлекаем их
		ret = FIFO_FRONT(uart_rx_fifo);
		FIFO_POP(uart_rx_fifo);
	}
	else {
		ret = _FDEV_EOF; //данных нет
	}
	sei(); //разрешаем прерывания
	return ret;
}















