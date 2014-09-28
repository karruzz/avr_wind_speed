//***************************************************************************
//  File........: lcd_lib.h
//
//  Author(s)...: Pashgan    chipenable.ru
//
//  Target(s)...: ATMega...
//
//  Compiler....: GNU Compiler Collection (GCC) 4.3.2 
//
//  Description.: Драйвер знакосинтезирующего жк дисплея
//
//  Data........: 28.10.09  
//
//***************************************************************************
#ifndef LCD_LIB_H
#define LCD_LIB_H

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

//порт к которому подключена шина данных ЖКД
#define PORT_DATA PORTB
#define PIN_DATA  PINB
#define DDRX_DATA DDRB

//порт к которому подключены управляющие выводы ЖКД
#define PORT_SIG PORTD
#define PIN_SIG  PIND
#define DDRX_SIG DDRD

//Номера выводов к которым подключены управляющие выводы ЖКД 
#define RS 5
#define RW 6
#define EN 7

#define CHECK_FLAG_BF
#define BUS_4BIT
#define HD44780
//******************************************************************************
//макросы

/*позиционирование курсора*/
#define LCD_Goto(x,y) LCD_WriteCom(((((y)& 1)*0x40)+((x)& 7))|128) 

//прототипы функций
void LCD_Init(void);//инициализация портов и жкд
void LCD_Clear(void);//очистка дисплея
int LCD_WriteData(char c, FILE *unused); //выводит байт данных на жкд
void LCD_WriteCom(unsigned char data); //посылает команду жкд
void LCD_SendStringFlash(const char *str); 
void LCD_SendString(const char *str);
#endif