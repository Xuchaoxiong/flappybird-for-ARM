/*
 * TIVA Cortex M4 MCU Experiment Application
 * Copyright (c) 2013-2014 China JiLiang University
 * All Rights Reserved.
 */

/*!
 * @file        LCDDriver.c
 * @version     1.0
 * @author      Kai Zhang
 * @date        2013-12-21
 * @brief       LCDÏÔÊ¾Çý¶¯³ÌÐò
 *
 */

#ifndef PART_TM4C123GH6PM
#define PART_TM4C123GH6PM
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "LCDDriver.h"
#include "LCD_Matrix.h"
#include "driverlib/ssi.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "inc/hw_ints.h"



/*     _________
 * 				|
 * 			 PC6|-->SCK		´®ÐÐÊ±ÖÓ
 *			 PC7|-->SDA		Êý¾Ý´«Êä
 *	TIVA  	 PB1|-->CS		µÍµçÆ½Æ¬Ñ¡
 *			 PD1|-->CD		Êý¾Ý/Ö¸Áî¼Ä´æÆ÷
 *			 PE5|-->RST		µÍµçÆ½¸´Î»
 *	   _________|
 */
// µÍµçÆ½Æ¬Ñ¡
#define setCS  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);	//PB1
#define rstCS  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);

// ¼Ä´æÆ÷Ñ¡ÔñÐÅºÅ
#define setCD  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);	//PC6
#define rstCD  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);

// µÍµçÆ½¸´Î»£¬¸´Î»Íê³Éºó£¬»Øµ½¸ßµçÆ½£¬Òº¾§Ä£¿é¿ªÊ¼¹¤×÷
#define setRES GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);	//PE5
#define rstRES GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);


//###############################################################################################

/******************************************************************************
 * @brief  ÇåÆÁº¯Êý£¬Çå³þËùÓÐµÄÏÔÊ¾ÄÚÈÝ
 * @param
 * @return none
 ******************************************************************************/
void LCD_ScreenClr() {
	unsigned char i, j;
	for (i = 0; i < 9; i++)				//Ñ­»·µÚ0~8Ò³Ãæ
			{
		rstCS
		LCD_TransferCmd(0xb0 + i);		//Ñ¡Ôñ¡°Ò³¡±
		LCD_TransferCmd(0x10);			//Ñ¡Ôñ¡°ÁÐ¡±
		LCD_TransferCmd(0x00);
		for (j = 0; j < 132; j++) {
			LCD_TransferData(0x00);		//ÊäÈëÊý¾Ý0x00£¬ÏÔÊ¾0x00
		}
	}
}

/******************************************************************************
 * @brief  Òº¾§Îå¸ö¶Ë¿Ú³õÊ¼»¯
 * @param
 * @return none
 ******************************************************************************/
void LCD_Enable() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);					//¸÷¸ö¶Ë¿ÚÊ¹ÄÜ
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//	-----------------------------------------------------------------------------
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);		//¸÷¸ö¶Ë¿ÚÉèÖÃÎªÊä³ö£¬PB1->CS
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);				//PC6->CD
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);				//PE5->RST

//	-----------------------------------------------------------------------------

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);						//ÏµÍ³ÍâÉèÊ¹ÄÜ
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinConfigure(GPIO_PD0_SSI3CLK);								//PD0£¬PD3Á½Ïß
	GPIOPinConfigure(GPIO_PD3_SSI3TX);

	GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_3);		//SSI¶Ë¿Ú¹¦ÄÜÊ¹ÄÜ

	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, 50000, 8);	//SSI3,¶Ë¿ÚÄ£Ê½:50K,8Î»Êý¾Ý£¬SSI_FRF_MOTO_MODE_0Ä£Ê½

	SSIEnable(SSI3_BASE);
//  -----------------------------------------------------------------------------
}

/******************************************************************************
 * @brief  ½Ï³¤Ê±¼äÑÓÊ±
 * @param
 * @return none
 ******************************************************************************/
void LCD_Delay2(unsigned int i) {
	int j, k;
	for (j = 0; j < i; j++)
		for (k = 0; k < 990; k++)
			;
}

/******************************************************************************
 * @brief  ½Ï¶ÌÊ±¼äÑÓÊ±
 * @param
 * @return none
 ******************************************************************************/
void LCD_Delay3(unsigned int i) {
	int j, k;
	for (j = 0; j < i; j++)
		for (k = 0; k < 10; k++)
			;
}

/******************************************************************************
 * @brief  	ÏÔÊ¾8*8µÄ¾ØÕóÏñËØ
 * @param	data £ºÊäÈëµÄÏÔÊ¾Êý¾ÝÖ¸Õë
 * @param	type £ºÊÇ·ñÐèÒª·´ÏàÏÔÊ¾
 * @return  none
 ******************************************************************************/
void LCD_TransferData(unsigned char data) {

	rstCS
	setCD

	while (SSIBusy(SSI3_BASE))
		;
	SSIDataPut(SSI3_BASE, data);
	while (SSIBusy(SSI3_BASE))
		;
}

/******************************************************************************
 * @brief  	·¢ËÍÒº¾§¿ØÖÆÃüÁî
 * @param	command £ºÃüÁî×Ö·û
 * @return  none
 ******************************************************************************/
void LCD_TransferCmd(unsigned char command) {

	rstCS
	rstCD

	while (SSIBusy(SSI3_BASE))
		;
	SSIDataPut(SSI3_BASE, command);
	while (SSIBusy(SSI3_BASE))
		;
}

/******************************************************************************
 * @brief  	³õÊ¼»¯Òº¾§
 * @param	none
 * @return  none
 ******************************************************************************/
void LCD_Init() {
	rstRES
	LCD_Delay2(50);                 //delay
	setRES
	LCD_Delay2(50);                 //delay
	LCD_TransferCmd(0xe2); /*Èí¸´Î»*/
	LCD_TransferCmd(0x2c); /*ÉýÑ¹²½¾Û1*/
	LCD_TransferCmd(0x2e); /*ÉýÑ¹²½¾Û2*/
	LCD_TransferCmd(0x2f); /*ÉýÑ¹²½¾Û3*/
	LCD_TransferCmd(0x23); /*´Öµ÷¶Ô±È¶È£¬¿ÉÉèÖÃ·¶Î§20¡«27*/
	LCD_TransferCmd(0x81); /*Î¢µ÷¶Ô±È¶È*/
	LCD_TransferCmd(0x28); /*Î¢µ÷¶Ô±È¶ÈµÄÖµ£¬¿ÉÉèÖÃ·¶Î§0¡«63*/
	LCD_TransferCmd(0xa2); /*1/9 Æ«Ñ¹±È£¨bias£©*/
	LCD_TransferCmd(0xc8); /*ÐÐÉ¨ÃèË³Ðò£º´ÓÉÏµ½ÏÂc0 c8 */
	LCD_TransferCmd(0xa0); /*ÁÐÉ¨ÃèË³Ðò£º´Ó×óµ½ÓÒ a0 a1*/
	LCD_TransferCmd(0xa6); /*ÕýÏÔ 0xa6£¬·´ÏÔ 0xa7*/
	LCD_TransferCmd(0xaf); /*¿ªÏÔÊ¾*/

}

/******************************************************************************
 * @brief  	¶ÔÒº¾§µÄ8*16×ÖÄ£¾ØÕó×öË³Ê±Õë90¶ÈµÄÐý×ª£¬À´ÌùºÏÒº¾§µÄÏÔÊ¾Ä£Ê½
 * @param	c	ÐèÒªÏÔÊ¾µÄÊý×Ö»ò×Ö·û
 * @param	row	×Ö·ûÏÔÊ¾µÄÐÐÊý È¡Öµ·¶Î§0~8
 * @param	col	×Ö·ûÏÔÊ¾µÄÁÐÊý È¡Öµ·¶Î§0~21
 * @param	IsReverse	×Ö·ûÊÇ·ñ·­×ªÏÔÊ¾
 * @return  0 £¬ ±íÊ¾ÊäÈë²ÎÊý³ö´í
 *			1£¬	 ±íÊ¾ÏÔÊ¾³É¹¦
 ******************************************************************************/
unsigned char LCD_Draw_Char_8x16(char c, unsigned char row, unsigned char col) {

	unsigned char * add = asc + (c - 0x20) * 16;
	unsigned int i, j;							//¼ÆÊýÆ÷
	unsigned char col_0 = col;					// µÍËÄÎ»
	unsigned char col_1 = col >> 4;  			// ¸ßËÄÎ»

	//----------------ÊäÈë¼ì²â-------------
	//--------------·µ»Ø0±íÊ¾ÊäÈë³ö´í---------------
	if (col > LCD_MAX_COL - 1) {
		return 0;
	}
	if (row > LCD_MAX_RAW - 1) {
		return 0;
	}

	for (i = 0; i < 2; i++) {
		rstCS
		LCD_TransferCmd(0xb0 + row * 2 + i);            //ÉèÖÃ¡°Ò³¡±
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄ¸ßËÄÎ»
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄµÍËÄÎ»
		for (j = 0; j < 8; j++) {
			LCD_TransferData(*add);
			add++;
		}
	}

	return 1; 								//·µ»Ø1±íÊ¾Ö´ÐÐ³É¹¦£¡
}

/******************************************************************************
 * @brief  	¶ÔÒº¾§µÄ8*8×ÖÄ£¾ØÕó×öË³Ê±Õë90¶ÈµÄÐý×ª£¬À´ÌùºÏÒº¾§µÄÏÔÊ¾Ä£Ê½
 * @param	c	ÐèÒªÏÔÊ¾µÄÊý×Ö»ò×Ö·û
 * @param	row	×Ö·ûÏÔÊ¾µÄÐÐÊý È¡Öµ·¶Î§0~8
 * @param	col	×Ö·ûÏÔÊ¾µÄÁÐÊý È¡Öµ·¶Î§0~21
 * @return  0 £¬ ±íÊ¾ÊäÈë²ÎÊý³ö´í
 *			1£¬ ±íÊ¾ÏÔÊ¾³É¹¦
 ******************************************************************************/
unsigned char LCD_Draw_Char_8x8(char c, unsigned char row, unsigned char col,
		bool IsReverse) {

	unsigned char * add = acFont8ASCII[c - 0x20];
	unsigned int i, j;							//¼ÆÊýÆ÷
	unsigned char col_1 = col * CHAR_WIDTH >> 4;  			// ¸ßËÄÎ»
	unsigned char col_0 = col * CHAR_WIDTH;  				// ¸ßËÄÎ»
	unsigned char data_ori[NUM_CHAR][NUM_CHAR] = { 0 };
	unsigned char data_cha[NUM_CHAR][NUM_CHAR] = { 0 };
	unsigned char show_chr[NUM_CHAR] = { 0 };
	unsigned char ori_chr[NUM_CHAR] = { 0 };
	//----------------ÊäÈë¼ì²â-------------
	//--------------·µ»Ø0±íÊ¾ÊäÈë³ö´í---------------
	if (col > LCD_MAX_COL) {
		return 0;
	}
	if (row > ENUM_LINE_NUM) {
		return 0;
	}

	rstCS
	LCD_TransferCmd(0xb0 + row);            		//ÉèÖÃ¡°Ò³¡±
	LCD_TransferCmd(0x10 + (col_1 & 0x0f));			 //ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄ¸ßËÄÎ»
	LCD_TransferCmd(0x00 + (col_0 & 0x0f));			 //ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄµÍËÄÎ»

	for (i = 0; i < NUM_CHAR; i++) {
		ori_chr[i] = *add;
		for (j = 0; j < NUM_CHAR; j++) {
			data_ori[i][NUM_CHAR - j] = (char) (ori_chr[i] >> j) & 0x01;
		}
		add++;
	}

	for (i = 0; i < NUM_CHAR; i++) {
		for (j = 0; j < NUM_CHAR; j++) {
			data_cha[j][i] = data_ori[i][j];
		}
	}

	for (i = 0; i < NUM_CHAR; i++) {
		unsigned char temp = 0;
		for (j = 0; j < NUM_CHAR; j++) {
			if (IsReverse) {
				if (data_cha[i][j] == 0)
					temp = temp | (0x01 << j);
				else
					temp = temp | (0x00 << j);
			} else {
				if (data_cha[i][j] == 1)
					temp = temp | (0x01 << j);
				else
					temp = temp | (0x00 << j);
			}
		}
		show_chr[i] = temp;
	}

	for (j = 0; j < NUM_CHAR; j++) {
		LCD_TransferData(show_chr[j]);
//		add++;
	}

	return 1; 								//·µ»Ø1±íÊ¾Ö´ÐÐ³É¹¦£¡
}

/******************************************************************************
 * @brief  	ÏÔÊ¾Ò»´®8*8´óÐ¡µÄ×Ö·û´®
 * @param	c	ÐèÒªÏÔÊ¾µÄÊý×Ö»ò×Ö·û
 * @param	raw	×Ö·ûÏÔÊ¾µÄÐÐÊý È¡Öµ·¶Î§0~8
 * @param	col	×Ö·ûÏÔÊ¾µÄÁÐÊý È¡Öµ·¶Î§0~21
 * @param	IsReverse	×Ö·ûÊÇ·ñ·­×ªÏÔÊ¾
 * @return  0 £¬ ±íÊ¾ÊäÈë²ÎÊý³ö´í
 *			1£¬	 ±íÊ¾ÏÔÊ¾³É¹¦
 ******************************************************************************/
unsigned char LCD_Draw_String(char * c, unsigned int raw, unsigned int col,
		bool IsReverse) {
	int i = 0;

	int len = strlen(c);

	for (i = 0; i < len; i++) {
		LCD_Draw_Char_8x8(*c, raw, (col + i), IsReverse);
		c++;
	}
	return 0;
}


/******************************************************************************
 * @brief  	ÏÔÊ¾Ò»¸ö¸¡µãÐÍµÄÊý×Ö
 * @param	c	ÐèÒªÏÔÊ¾µÄÊý×Ö»ò×Ö·û
 * @param	raw	×Ö·ûÏÔÊ¾µÄÐÐÊý È¡Öµ·¶Î§0~8
 * @param	col	×Ö·ûÏÔÊ¾µÄÁÐÊý È¡Öµ·¶Î§0~21
 * @param	IsReverse	×Ö·ûÊÇ·ñ·­×ªÏÔÊ¾
 * @return  0 £¬ ±íÊ¾ÊäÈë²ÎÊý³ö´í
 *		   len:·µ»ØÊý¾Ý³¤¶ÈÖµ£¬¸ÃÖµÔÚÁ½´ÎÊý¾ÝÏÔÊ¾È·¶¨Çå³ýÎ»Ê±Ê¹ÓÃ
 ******************************************************************************/
unsigned char LCD_Draw_Num(float number, int scale, unsigned int raw,
		unsigned int col, bool IsReverse) {

	//Í¨¹ýÐ¡Êýµã±£ÁôÎ»ÊýËãËù³ËÏµÊý
	unsigned char zero = '0';

	int show_number = 0;
	int temp = show_number;
	int Cur_len = 0;					//µ±Ç°Êý¾Ý³¤¶È
	int multi = 1;
	int i = 0;
			
		if (scale > 10 || scale < 0)
		return 0;		
	
	if(scale > 0 ){

		for (i = 0; i < scale; i++) {
			multi *= 10;
		}
	}else{
		multi = 1;
	}
		// ¼ÆËãÏÔÊ¾µÄÊý×ÖÎ»Êý
	show_number = (int) (number * multi);
	temp = show_number;
	for (i = 0; i < 10; i++) {
		if (i == 9)	// Êý¾Ý¹ý´ó·µ»Ø
			return 0;

		temp = temp / 10;
		Cur_len++;
		if (temp == 0) {
			break;
		}
	}

	//´¦ÀíÐ¡ÓÚ1µÄÊý¾ÝµÄÕý³£ÏÔÊ¾£¬ÄÜ´¦Àí×îÐ¡ÊýÖµ0.001.
	if(number < 1.0 && number > 0.1)
	{
		Cur_len = Cur_len + 1;
	}
	else if(number < 0.1 && number >0.01)
	{
		Cur_len = Cur_len +2;
	}
	else if(number < 0.001 && number > 0)
	{
		Cur_len = Cur_len +3;
	}

	//ÏÔÊ¾Êý×Ö
	for (i = Cur_len - 1; i >= 0; i--) {

		if (scale == 0)		//ÏÔÊ¾ÕûÊý
		{

			LCD_Draw_Char_8x8(zero + show_number % 10, raw, (col + i), IsReverse);
		} else {				//´øÐ¡ÊýµãÏÔÊ¾
			if (i < Cur_len - scale) {
				LCD_Draw_Char_8x8(zero + show_number % 10, raw, (col + i),
						IsReverse);
			} else {
				LCD_Draw_Char_8x8(zero + show_number % 10, raw, (col + i + 1),
						IsReverse);
			}

		}
		show_number = show_number / 10;
	}

	if (scale != 0 && Cur_len > scale)
		LCD_Draw_Char_8x8('.', raw, (col + Cur_len - scale), IsReverse);

	if(scale != 0)
	{
		Cur_len = Cur_len + 1;
	}
	return Cur_len;
}

void LCD_Draw_Line_Clear(int line, bool IsReverse) {
	unsigned char j;

	rstCS
	LCD_TransferCmd(0xb0 + line);		//Ñ¡Ôñ¡°Ò³¡±
	LCD_TransferCmd(0x10);				//Ñ¡Ôñ¡°ÁÐ¡±
	LCD_TransferCmd(0x00);
	for (j = 0; j < 132; j++) {
		if (IsReverse)
			LCD_TransferData(0xFF);		//ÊäÈëÊý¾Ý0xFF£¬ÏÔÊ¾0xFF
		else
			LCD_TransferData(0x00);		//ÊäÈëÊý¾Ý0x00£¬ÏÔÊ¾0x00
	}
}

void LCD_Draw_Clear_8x8(unsigned int page,unsigned int col)
{
	unsigned char j;
	unsigned char col_1 = col * CHAR_WIDTH >> 4;  			// ¸ßËÄÎ»
	unsigned char col_0 = col * CHAR_WIDTH;  				// ¸ßËÄÎ»
	rstCS
	LCD_TransferCmd(0xb0 + page);		//Ñ¡Ôñ¡°Ò³¡±
	LCD_TransferCmd(0x10+(col_1 & 0x0f));		//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄ¸ßËÄÎ»
	LCD_TransferCmd(0x00+(col_0 & 0x0f));		//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄµÍËÄÎ»
	for (j = 0; j < NUM_CHAR; j++) {
			LCD_TransferData(0x00);		//ÊäÈëÊý¾Ý0x00£¬ÏÔÊ¾0x00
		}
}

/******************************************************************************
 * @brief  	¶ÔÒº¾§µÄ8*8×ÖÄ£¾ØÕó×öË³Ê±Õë90¶ÈµÄÐý×ª£¬À´ÌùºÏÒº¾§µÄÏÔÊ¾Ä£Ê½
 * @param	index	ÐèÒªÏÔÊ¾µÄºº×ÖµÄ×ÖÄ£±àºÅ
 * @param	row	×Ö·ûÏÔÊ¾µÄÐÐÊý ,È¡Öµ·¶Î§0~3
 * @param	col	×Ö·ûÏÔÊ¾µÄÁÐÊý ,È¡Öµ·¶Î§0~119
 * @return  0 £¬ ±íÊ¾ÊäÈë²ÎÊý³ö´í
 *			1£¬ ±íÊ¾ÏÔÊ¾³É¹¦
 ******************************************************************************/
unsigned char LCD_Draw_Chinese(unsigned char index, unsigned char row,
		unsigned char col) {
	unsigned char * chinese = CHINESE_MATRIX + 32 * index;
	unsigned int i, j;					//¼ÆÊýÆ÷
	unsigned char col_0 = col;		//ÁÐµÍËÄÎ»
	unsigned char col_1 = col >> 4;  //ÁÐ¸ßËÄÎ»

	//----------------ÊäÈë¼ì²â-------------
	//-----------·µ»Ø0±íÊ¾ÊäÈë³ö´í------------
	if (col > 111) {
		return 0;
	}
	if (row > 3) {
		return 0;
	}

	for (i = 0; i < 2; i++) {
		rstCS
		LCD_TransferCmd(0xb0 + row);             //ÉèÖÃ¡°Ò³¡±
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			 //ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄ¸ßËÄÎ»
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			 //ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄµÍËÄÎ»
		for (j = 0; j < 16; j++) {
			LCD_TransferData(*chinese);
			chinese++;
		}
	}
	return 1; //·µ»Ø1±íÊ¾Ö´ÐÐ³É¹¦£¡
}

/************************************************************************
 * ³õÊ¼»¯ADC»ñÈ¡¹öÂÖµçÑ¹Öµ
 * 		¡ª¡ª¡ª¡ª¡ª¡ª¡ª¡ª
 * 				|
 //			 PC7|<--Button1
 * 	TIVA	 PD6|<--Button2
 * 			 PD7|<--Button3
 //		________|
 *
 *  @return	0x00
 *  		0x01
 *  		0x02
 ***********************************************************************/
unsigned char scan_key(void) {
	int32_t ping1 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6);
	int32_t ping2 = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7);
	int32_t ping3 = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7);
	if (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7) == 0x00) {

		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ÑÓÊ±Ô¼10ms£¬Ïû³ý°´¼ü¶¶¶¯
		while (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7) == 0x00)
			; 		// µÈ´ýKEYÌ§Æð
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ÑÓÊ±Ô¼10ms£¬Ïû³ýËÉ¼ü¶¶¶¯

		// 	do something

		return 0x01;
	}

	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == 0x00) {
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ÑÓÊ±Ô¼10ms£¬Ïû³ý°´¼ü¶¶¶¯
		while (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == 0x00)
			; 		// µÈ´ýKEYÌ§Æð
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ÑÓÊ±Ô¼10ms£¬Ïû³ýËÉ¼ü¶¶¶¯

		// 	do something

		return 0x02;
	}

//	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7) == 0x00)
//	{
//		SysCtlDelay(10*(SysCtlClockGet() / 3000)); 					// ÑÓÊ±Ô¼10ms£¬Ïû³ý°´¼ü¶¶¶¯
//		while (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7) == 0x00); 		// µÈ´ýKEYÌ§Æð
//		SysCtlDelay(10*(SysCtlClockGet() / 3000)); 					// ÑÓÊ±Ô¼10ms£¬Ïû³ýËÉ¼ü¶¶¶¯
//		return 0x03;
//	}

	return 0;
}


void screenpage(char page,unsigned char *p,int l)
{		int j;
	rstCS
	LCD_TransferCmd(0xb0 + page);		//Ñ¡Ôñ¡°Ò³¡±
	LCD_TransferCmd(0x10+(0 & 0x0f));		//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄ¸ßËÄÎ»
	LCD_TransferCmd(0x00+(0 & 0x0f));		//ÉèÖÃ¡°ÁÐ¡±ÒÔ¼°ÁÐµÄµÍËÄÎ»
	for (j = 0; j < l; j++) {
			LCD_TransferData(p[j]);		//ÊäÈëÊý¾Ý£¬ÏÔÊ¾Êý¾Ý
	}
}
/******************************************************************************
 * @brief  	????8*8????????90????,??????????
 * @param	c	??????????
 * @param	row	??????? ????0~3
 * @param	col	??????? ????0~119
 * @return  0 , ????????
 *			1, ??????
 ******************************************************************************/
unsigned char LCD_Draw_Char(char c, unsigned char row, unsigned char col) {

	unsigned char * add = asc + (c - 0x20) * 16;
	unsigned int i,j;							//???
	unsigned char col_0 = col;					// ???
	unsigned char col_1 = col >> 4;  			// ???

	//----------------????-------------
	//--------------??0??????---------------
	if (col > LCD_MAX_COL - 1) {
		return 0;
	}
	if (row > LCD_MAX_RAW - 1) {
		return 0;
	}

	for (i = 0; i < 2; i++)
	{
		rstCS
		LCD_TransferCmd(0xb0 + row * 2 + i);            //??“?”
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			//??“?”???????
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			//??“?”???????
		for (j = 0; j < 8; j++)
		{
			LCD_TransferData(*add);
			add++;
		}
	}

	return 1; 								//??1??????!
}
