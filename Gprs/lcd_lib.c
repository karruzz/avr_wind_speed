#include "lcd_lib.h"

//������� ��� ������ � ������
#define ClearBit(reg, bit)       reg &= (~(1<<(bit)))
#define SetBit(reg, bit)         reg |= (1<<(bit))	

#define FLAG_BF 7

inline unsigned char __swap_nibbles(unsigned char data)
{
  asm volatile("swap %0" : "=r" (data) : "0" (data));
  return data;
}

void LCD_WriteComInit(unsigned char data)
{
  _delay_us(40);
  ClearBit(PORT_SIG, RS);	
#ifdef BUS_4BIT
  unsigned char tmp; 
  data = __swap_nibbles(data);
  tmp  = PORT_DATA & 0xF0;
  tmp |= (data & 0x0F);
  PORT_DATA = tmp;		 
#else
  PORT_DATA = data;		
#endif  
  SetBit(PORT_SIG, EN);	        
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	
}


inline static void LCD_CommonFunc(unsigned char data)
{
#ifdef BUS_4BIT  
  unsigned char tmp; 
  data = __swap_nibbles(data);
  tmp  = PORT_DATA & 0xF0;
  tmp |= (data & 0x0F);

  PORT_DATA = tmp;		//����� ������� ������� 
  SetBit(PORT_SIG, EN);	        
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	

  data = __swap_nibbles(data);
  tmp  = PORT_DATA & 0xF0;
  tmp |= (data & 0x0F);
 
  PORT_DATA = tmp;		//����� ������� ������� 
  SetBit(PORT_SIG, EN);	        
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	 
#else 
  PORT_DATA = data;		    //����� ������ �� ���� ���������� 
  SetBit(PORT_SIG, EN);	     //��������� E � 1
  _delay_us(2);
  ClearBit(PORT_SIG, EN);	//��������� E � 0 - ������������ �����
#endif
}

inline static void LCD_Wait(void)
{
#ifdef CHECK_FLAG_BF
  #ifdef BUS_4BIT
  
  unsigned char data;
  DDRX_DATA &= 0xF0;            //������������� ���� �� ����
  PORT_DATA |= 0x0F;	        //�������� pull-up ���������
  SetBit(PORT_SIG, RW);         //RW � 1 ������ �� lcd
  ClearBit(PORT_SIG, RS);	//RS � 0 �������
  do{
    data = __swap_nibbles(data);
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data = PIN_DATA & 0x0F;      //������ ������ � �����
    ClearBit(PORT_SIG, EN);
    data = __swap_nibbles(data);
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data |= PIN_DATA & 0x0F;      //������ ������ � �����
    ClearBit(PORT_SIG, EN);
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG, RW);
  DDRX_DATA |= 0x0F; 
  
  #else
  unsigned char data;
  DDRX_DATA = 0;                //������������� ���� �� ����
  PORT_DATA = 0xff;	        //�������� pull-up ���������
  SetBit(PORT_SIG, RW);         //RW � 1 ������ �� lcd
  ClearBit(PORT_SIG, RS);	//RS � 0 �������
  do{
    SetBit(PORT_SIG, EN);	
    _delay_us(2);
    data = PIN_DATA;            //������ ������ � �����
    ClearBit(PORT_SIG, EN);	
  }while((data & (1<<FLAG_BF))!= 0 );
  ClearBit(PORT_SIG, RW);
  DDRX_DATA = 0xff; 
  #endif    
#else
  _delay_us(40);
#endif  
}

//������� ������ ������� 
void LCD_WriteCom(unsigned char data)
{
  LCD_Wait();
  ClearBit(PORT_SIG, RS);	//��������� RS � 0 - �������
  LCD_CommonFunc(data);
}

//������� ������ ������
int LCD_WriteData(char c, FILE *unused)
{
  if (c == '\n' || c == '\r') return 0;
  
  LCD_Wait();
  SetBit(PORT_SIG, RS);	    //��������� RS � 1 - ������
  LCD_CommonFunc(c);
  return 0;
}

//������� �������������
void LCD_Init(void)
{
#ifdef BUS_4BIT
  DDRX_DATA |= 0x0F;
  PORT_DATA |= 0x0F; 
#else  
  DDRX_DATA |= 0xff;
  PORT_DATA |= 0xff;
#endif
  
  DDRX_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  PORT_SIG |= (1<<RW)|(1<<RS)|(1<<EN);
  ClearBit(PORT_SIG, RW);
  _delay_ms(40);
  
#ifdef HD44780  
  LCD_WriteComInit(0x30); 
  _delay_ms(10);
  LCD_WriteComInit(0x30);
  _delay_ms(1);
  LCD_WriteComInit(0x30);
#endif
  
#ifdef BUS_4BIT  
  LCD_WriteComInit(0x20); //4 ��������� ����
  LCD_WriteCom(0x28); //4-��������� ����, 2 - ������
#else
  LCD_WriteCom(0x38); //8-��������� ����, 2 - ������
#endif
  LCD_WriteCom(0x08);
  LCD_WriteCom(0x0c);  //������� ���, ������ � �������� ���������
  LCD_WriteCom(0x01);  //0b00000001 - ������� �������
  _delay_ms(2);
  LCD_WriteCom(0x06);  //0b00000110 - ������ �������� ������, ������ ���
}


//������� ������ ������ �� ���� ������
void LCD_SendStringFlash(const char *str)
{
  char c;
  while (c = pgm_read_byte(str++))
  {
	if (c == '\n' || c == '\r') return;	  
    LCD_Wait();
    SetBit(PORT_SIG, RS);			
    LCD_CommonFunc(c);
  }
}

//������ ������ ������ �� RAM
void LCD_SendString(const char *str)
{
  char c;
  SetBit(PORT_SIG, RS);			
  while (c = *str++)
  {
	if (c == '\n' || c == '\r') return;	  
    LCD_Wait();
    SetBit(PORT_SIG, RS);			
    LCD_CommonFunc(c);
  }
}


void LCD_Clear(void)
{
  LCD_WriteCom(0x01);
  _delay_ms(2);
}