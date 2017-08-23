/*
 * TIVA Cortex M4 MCU Experiment Application
 * Copyright (c) 2013-2014 China JiLiang University
 * All Rights Reserved.
 */

/*!
 * @file        LCDDriver.h
 * @version     1.0
 * @author      Kai Zhang
 * @date        2013-12-21
 * @brief       LCD��ʾ����ͷ�ļ�
 *
 */
//###############################################################################################
/*	   _________
 *				|
 *			 PC6|-->SCK		����ʱ��
 *			 PC7|-->SDA		���ݴ���
 *	TIVA  	 PB1|-->CS		�͵�ƽƬѡ
 *			 PD1|-->CD		����/ָ��Ĵ���
 *			 PE5|-->RST		�͵�ƽ��λ
 *			 	|
 *			 PC6|<--Button1
 * 			 PD6|<--Button2
 * 			 PD7|<--Button3
 * 			    |
 * 			 PE0|<--ADC		���ֵ���ADC����
 *		________|
 *
 *
 *
 */
#ifndef LCD_H_
#define LCD_H_
//###############################################################################################
// ͷ�ļ�����
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

//�궨��
#define LCD_DRAW_NORMAL				0		// ������ʾ
#define LCD_DRAW_HIGHLIGHT			1		// ������ʾ
#define LCD_MAX_RAW					8		// Һ���������
#define NUM_CHAR					8
#define CHAR_WIDTH					7
#define NUM_WIDTH					7
#define LCD_MAX_COL					18		// Һ���������
#define WORD_WIDTH					16
//------------------------------------------------------------------------------------------
//Һ����ʾ��ر����Ķ���
enum
{
	ENUM_LINE_ONE,
	ENUM_LINE_TWO,
	ENUM_LINE_THREE,
	ENUM_LINE_FOUR,
	ENUM_LINE_FIVE,
	ENUM_LINE_SIX,
	ENUM_LINE_SEVEN,
	ENUM_LINE_EIGHT,

	ENUM_LINE_NUM
};


enum
{
	DIRECTOR_RAW,
	DIRECTOR_COL,

	DIRECTOR_NUM
};//director_define;

enum
{
	SSI1_UNITS = 101,
	SSI1_TENS = 85,
	SSI1_HUNDREDS = 69,
	SSI1_THOUSANDS = 53
};//cursor_define_for_MDAC;




//uint32_t cur_position = SSI1_UNITS , cur_position_old = SSI1_UNITS;

//unsigned char clr = ' ' , arrow_l = '[', arrow_r = ']';

//uint32_t pui32ADC0Value[1];

//------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------
//����������
// LCD����
void LCD_ScreenClr(void);

// ʹ��LCD���õ��Ķ˿�
void LCD_Enable(void);

// ��ʱ
void LCD_Delay2(unsigned int i);

// ��ʱ
void LCD_Delay3(unsigned int i);

// ��ʾ��ģ
void LCD_TransferData(unsigned char data);

// LCD����ָ��
void LCD_TransferCmd(unsigned char command);

// LCD��ʼ��
void LCD_Init(void);

// ��ʾ8*16��ģ
unsigned char LCD_Draw_Char_8x16(char c,unsigned char page,unsigned char col);

// ��ʾ8*8��ģ
unsigned char LCD_Draw_Char_8x8(char c, unsigned char page,unsigned char col, bool IsReverse);

// ��ʾ�ַ���
unsigned char LCD_Draw_String(char * c, unsigned int page,unsigned int col, bool IsReverse);

// ��ʾ����
unsigned char LCD_Draw_Num(float number, int scale, unsigned int page, unsigned int col, bool IsReverse);

void LCD_Draw_Line_Clear(int line, bool IsReverse);

void LCD_Draw_Clear_8x8(unsigned int page,unsigned int col);

// ��ʾ8*8������ģ
void LCD_Draw_Byte(bool b_invert);

// ��ʾ����
unsigned char LCD_Draw_Chinese(unsigned char index, unsigned char raw, unsigned char col);

// ������ת����
void LCD_InvertLight_All(void);

// ȫ����ʾͼ��
void DisplayGraphic1(void);

void Init_Int_Key(void);

void Init_ADCWheel(void);

unsigned char scan_key(void);
// ??8*16??
unsigned char LCD_Draw_Char(char c,unsigned char page,unsigned char col);

#endif
//-------------------------------------------------------------------------
