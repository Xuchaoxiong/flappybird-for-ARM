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
 * @brief       LCD显示驱动头文件
 *
 */
//###############################################################################################
/*	   _________
 *				|
 *			 PC6|-->SCK		串行时钟
 *			 PC7|-->SDA		数据传输
 *	TIVA  	 PB1|-->CS		低电平片选
 *			 PD1|-->CD		数据/指令寄存器
 *			 PE5|-->RST		低电平复位
 *			 	|
 *			 PC6|<--Button1
 * 			 PD6|<--Button2
 * 			 PD7|<--Button3
 * 			    |
 * 			 PE0|<--ADC		滚轮电阻ADC采样
 *		________|
 *
 *
 *
 */
#ifndef LCD_H_
#define LCD_H_
//###############################################################################################
// 头文件声明
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"

//宏定义
#define LCD_DRAW_NORMAL				0		// 正常显示
#define LCD_DRAW_HIGHLIGHT			1		// 背光显示
#define LCD_MAX_RAW					8		// 液晶最大行数
#define NUM_CHAR					8
#define CHAR_WIDTH					7
#define NUM_WIDTH					7
#define LCD_MAX_COL					18		// 液晶最大列数
#define WORD_WIDTH					16
//------------------------------------------------------------------------------------------
//液晶显示相关变量的定义
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
//函数包括：
// LCD清屏
void LCD_ScreenClr(void);

// 使能LCD所用到的端口
void LCD_Enable(void);

// 延时
void LCD_Delay2(unsigned int i);

// 延时
void LCD_Delay3(unsigned int i);

// 显示字模
void LCD_TransferData(unsigned char data);

// LCD发送指令
void LCD_TransferCmd(unsigned char command);

// LCD初始化
void LCD_Init(void);

// 显示8*16字模
unsigned char LCD_Draw_Char_8x16(char c,unsigned char page,unsigned char col);

// 显示8*8字模
unsigned char LCD_Draw_Char_8x8(char c, unsigned char page,unsigned char col, bool IsReverse);

// 显示字符串
unsigned char LCD_Draw_String(char * c, unsigned int page,unsigned int col, bool IsReverse);

// 显示数字
unsigned char LCD_Draw_Num(float number, int scale, unsigned int page, unsigned int col, bool IsReverse);

void LCD_Draw_Line_Clear(int line, bool IsReverse);

void LCD_Draw_Clear_8x8(unsigned int page,unsigned int col);

// 显示8*8矩阵字模
void LCD_Draw_Byte(bool b_invert);

// 显示汉字
unsigned char LCD_Draw_Chinese(unsigned char index, unsigned char raw, unsigned char col);

// 整屏翻转设置
void LCD_InvertLight_All(void);

// 全屏显示图形
void DisplayGraphic1(void);

void Init_Int_Key(void);

void Init_ADCWheel(void);

unsigned char scan_key(void);
// ??8*16??
unsigned char LCD_Draw_Char(char c,unsigned char page,unsigned char col);

#endif
//-------------------------------------------------------------------------
