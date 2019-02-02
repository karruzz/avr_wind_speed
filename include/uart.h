/*
 * Uart.h
 *
 * Created: 20.09.2014 18:40:59
 *  Author: karruzz
 * License: GNU GPL 3
 */

#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "fifo.h"

void uart_init(uint16_t ubrr);
int uart_putc(char c, FILE *file);
int uart_getc(FILE* file);

#endif /* UART_H_ */
