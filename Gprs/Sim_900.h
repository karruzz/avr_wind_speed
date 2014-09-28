/*
 * Sim_900.h
 *
 * Created: 21.09.2014 10:14:45
 *  Author: karruzz
 * License: GNU GPL 3
 */

#ifndef F_CPU
#define F_CPU 7372800UL
#endif

#ifndef SIM_900_H_
#define SIM_900_H_

#include <stdio.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

void Sim900PowerOn(void);
void Sim900Init();
void Sim900SendTemp();
void Sim900PowerOff(void);

#endif /* SIM_900_H_ */