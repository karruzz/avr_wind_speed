//***************************************************************************
//  File........: lcd_lib.h
//
//  Author(s)...: Pashgan    chipenable.ru
//
//  Target(s)...: ATMega...
//
//  Compiler....: GNU Compiler Collection (GCC) 4.3.2 
//
//  Description.: ������� ������������������� �� �������
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

//���� � �������� ���������� ���� ������ ���
#define PORT_DATA PORTB
#define PIN_DATA  PINB
#define DDRX_DATA DDRB

//���� � �������� ���������� ����������� ������ ���
#define PORT_SIG PORTD
#define PIN_SIG  PIND
#define DDRX_SIG DDRD

//������ ������� � ������� ���������� ����������� ������ ��� 
#define RS 5
#define RW 6
#define EN 7

#define CHECK_FLAG_BF
#define BUS_4BIT
#define HD44780
//******************************************************************************
//�������

/*���������������� �������*/
#define LCD_Goto(x,y) LCD_WriteCom(((((y)& 1)*0x40)+((x)& 7))|128) 

//��������� �������
void LCD_Init(void);//������������� ������ � ���
void LCD_Clear(void);//������� �������
int LCD_WriteData(char c, FILE *unused); //������� ���� ������ �� ���
void LCD_WriteCom(unsigned char data); //�������� ������� ���
void LCD_SendStringFlash(const char *str); 
void LCD_SendString(const char *str);
#endif