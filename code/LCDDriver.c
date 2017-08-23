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
 * @brief       LCD��ʾ��������
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
 * 			 PC6|-->SCK		����ʱ��
 *			 PC7|-->SDA		���ݴ���
 *	TIVA  	 PB1|-->CS		�͵�ƽƬѡ
 *			 PD1|-->CD		����/ָ��Ĵ���
 *			 PE5|-->RST		�͵�ƽ��λ
 *	   _________|
 */
// �͵�ƽƬѡ
#define setCS  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, GPIO_PIN_1);	//PB1
#define rstCS  GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, 0);

// �Ĵ���ѡ���ź�
#define setCD  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, GPIO_PIN_6);	//PC6
#define rstCD  GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6, 0);

// �͵�ƽ��λ����λ��ɺ󣬻ص��ߵ�ƽ��Һ��ģ�鿪ʼ����
#define setRES GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, GPIO_PIN_5);	//PE5
#define rstRES GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5, 0);


//###############################################################################################

/******************************************************************************
 * @brief  ����������������е���ʾ����
 * @param
 * @return none
 ******************************************************************************/
void LCD_ScreenClr() {
	unsigned char i, j;
	for (i = 0; i < 9; i++)				//ѭ����0~8ҳ��
			{
		rstCS
		LCD_TransferCmd(0xb0 + i);		//ѡ��ҳ��
		LCD_TransferCmd(0x10);			//ѡ���С�
		LCD_TransferCmd(0x00);
		for (j = 0; j < 132; j++) {
			LCD_TransferData(0x00);		//��������0x00����ʾ0x00
		}
	}
}

/******************************************************************************
 * @brief  Һ������˿ڳ�ʼ��
 * @param
 * @return none
 ******************************************************************************/
void LCD_Enable() {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);					//�����˿�ʹ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
//	-----------------------------------------------------------------------------
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_1);		//�����˿�����Ϊ�����PB1->CS
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_6);				//PC6->CD
	GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_5);				//PE5->RST

//	-----------------------------------------------------------------------------

	SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI3);						//ϵͳ����ʹ��
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinConfigure(GPIO_PD0_SSI3CLK);								//PD0��PD3����
	GPIOPinConfigure(GPIO_PD3_SSI3TX);

	GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_3);		//SSI�˿ڹ���ʹ��

	SSIConfigSetExpClk(SSI3_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
			SSI_MODE_MASTER, 50000, 8);	//SSI3,�˿�ģʽ:50K,8λ���ݣ�SSI_FRF_MOTO_MODE_0ģʽ

	SSIEnable(SSI3_BASE);
//  -----------------------------------------------------------------------------
}

/******************************************************************************
 * @brief  �ϳ�ʱ����ʱ
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
 * @brief  �϶�ʱ����ʱ
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
 * @brief  	��ʾ8*8�ľ�������
 * @param	data ���������ʾ����ָ��
 * @param	type ���Ƿ���Ҫ������ʾ
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
 * @brief  	����Һ����������
 * @param	command �������ַ�
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
 * @brief  	��ʼ��Һ��
 * @param	none
 * @return  none
 ******************************************************************************/
void LCD_Init() {
	rstRES
	LCD_Delay2(50);                 //delay
	setRES
	LCD_Delay2(50);                 //delay
	LCD_TransferCmd(0xe2); /*��λ*/
	LCD_TransferCmd(0x2c); /*��ѹ����1*/
	LCD_TransferCmd(0x2e); /*��ѹ����2*/
	LCD_TransferCmd(0x2f); /*��ѹ����3*/
	LCD_TransferCmd(0x23); /*�ֵ��Աȶȣ������÷�Χ20��27*/
	LCD_TransferCmd(0x81); /*΢���Աȶ�*/
	LCD_TransferCmd(0x28); /*΢���Աȶȵ�ֵ�������÷�Χ0��63*/
	LCD_TransferCmd(0xa2); /*1/9 ƫѹ�ȣ�bias��*/
	LCD_TransferCmd(0xc8); /*��ɨ��˳�򣺴��ϵ���c0 c8 */
	LCD_TransferCmd(0xa0); /*��ɨ��˳�򣺴����� a0 a1*/
	LCD_TransferCmd(0xa6); /*���� 0xa6������ 0xa7*/
	LCD_TransferCmd(0xaf); /*����ʾ*/

}

/******************************************************************************
 * @brief  	��Һ����8*16��ģ������˳ʱ��90�ȵ���ת��������Һ������ʾģʽ
 * @param	c	��Ҫ��ʾ�����ֻ��ַ�
 * @param	row	�ַ���ʾ������ ȡֵ��Χ0~8
 * @param	col	�ַ���ʾ������ ȡֵ��Χ0~21
 * @param	IsReverse	�ַ��Ƿ�ת��ʾ
 * @return  0 �� ��ʾ�����������
 *			1��	 ��ʾ��ʾ�ɹ�
 ******************************************************************************/
unsigned char LCD_Draw_Char_8x16(char c, unsigned char row, unsigned char col) {

	unsigned char * add = asc + (c - 0x20) * 16;
	unsigned int i, j;							//������
	unsigned char col_0 = col;					// ����λ
	unsigned char col_1 = col >> 4;  			// ����λ

	//----------------������-------------
	//--------------����0��ʾ�������---------------
	if (col > LCD_MAX_COL - 1) {
		return 0;
	}
	if (row > LCD_MAX_RAW - 1) {
		return 0;
	}

	for (i = 0; i < 2; i++) {
		rstCS
		LCD_TransferCmd(0xb0 + row * 2 + i);            //���á�ҳ��
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			//���á��С��Լ��еĸ���λ
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			//���á��С��Լ��еĵ���λ
		for (j = 0; j < 8; j++) {
			LCD_TransferData(*add);
			add++;
		}
	}

	return 1; 								//����1��ʾִ�гɹ���
}

/******************************************************************************
 * @brief  	��Һ����8*8��ģ������˳ʱ��90�ȵ���ת��������Һ������ʾģʽ
 * @param	c	��Ҫ��ʾ�����ֻ��ַ�
 * @param	row	�ַ���ʾ������ ȡֵ��Χ0~8
 * @param	col	�ַ���ʾ������ ȡֵ��Χ0~21
 * @return  0 �� ��ʾ�����������
 *			1�� ��ʾ��ʾ�ɹ�
 ******************************************************************************/
unsigned char LCD_Draw_Char_8x8(char c, unsigned char row, unsigned char col,
		bool IsReverse) {

	unsigned char * add = acFont8ASCII[c - 0x20];
	unsigned int i, j;							//������
	unsigned char col_1 = col * CHAR_WIDTH >> 4;  			// ����λ
	unsigned char col_0 = col * CHAR_WIDTH;  				// ����λ
	unsigned char data_ori[NUM_CHAR][NUM_CHAR] = { 0 };
	unsigned char data_cha[NUM_CHAR][NUM_CHAR] = { 0 };
	unsigned char show_chr[NUM_CHAR] = { 0 };
	unsigned char ori_chr[NUM_CHAR] = { 0 };
	//----------------������-------------
	//--------------����0��ʾ�������---------------
	if (col > LCD_MAX_COL) {
		return 0;
	}
	if (row > ENUM_LINE_NUM) {
		return 0;
	}

	rstCS
	LCD_TransferCmd(0xb0 + row);            		//���á�ҳ��
	LCD_TransferCmd(0x10 + (col_1 & 0x0f));			 //���á��С��Լ��еĸ���λ
	LCD_TransferCmd(0x00 + (col_0 & 0x0f));			 //���á��С��Լ��еĵ���λ

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

	return 1; 								//����1��ʾִ�гɹ���
}

/******************************************************************************
 * @brief  	��ʾһ��8*8��С���ַ���
 * @param	c	��Ҫ��ʾ�����ֻ��ַ�
 * @param	raw	�ַ���ʾ������ ȡֵ��Χ0~8
 * @param	col	�ַ���ʾ������ ȡֵ��Χ0~21
 * @param	IsReverse	�ַ��Ƿ�ת��ʾ
 * @return  0 �� ��ʾ�����������
 *			1��	 ��ʾ��ʾ�ɹ�
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
 * @brief  	��ʾһ�������͵�����
 * @param	c	��Ҫ��ʾ�����ֻ��ַ�
 * @param	raw	�ַ���ʾ������ ȡֵ��Χ0~8
 * @param	col	�ַ���ʾ������ ȡֵ��Χ0~21
 * @param	IsReverse	�ַ��Ƿ�ת��ʾ
 * @return  0 �� ��ʾ�����������
 *		   len:�������ݳ���ֵ����ֵ������������ʾȷ�����λʱʹ��
 ******************************************************************************/
unsigned char LCD_Draw_Num(float number, int scale, unsigned int raw,
		unsigned int col, bool IsReverse) {

	//ͨ��С���㱣��λ��������ϵ��
	unsigned char zero = '0';

	int show_number = 0;
	int temp = show_number;
	int Cur_len = 0;					//��ǰ���ݳ���
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
		// ������ʾ������λ��
	show_number = (int) (number * multi);
	temp = show_number;
	for (i = 0; i < 10; i++) {
		if (i == 9)	// ���ݹ��󷵻�
			return 0;

		temp = temp / 10;
		Cur_len++;
		if (temp == 0) {
			break;
		}
	}

	//����С��1�����ݵ�������ʾ���ܴ�����С��ֵ0.001.
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

	//��ʾ����
	for (i = Cur_len - 1; i >= 0; i--) {

		if (scale == 0)		//��ʾ����
		{

			LCD_Draw_Char_8x8(zero + show_number % 10, raw, (col + i), IsReverse);
		} else {				//��С������ʾ
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
	LCD_TransferCmd(0xb0 + line);		//ѡ��ҳ��
	LCD_TransferCmd(0x10);				//ѡ���С�
	LCD_TransferCmd(0x00);
	for (j = 0; j < 132; j++) {
		if (IsReverse)
			LCD_TransferData(0xFF);		//��������0xFF����ʾ0xFF
		else
			LCD_TransferData(0x00);		//��������0x00����ʾ0x00
	}
}

void LCD_Draw_Clear_8x8(unsigned int page,unsigned int col)
{
	unsigned char j;
	unsigned char col_1 = col * CHAR_WIDTH >> 4;  			// ����λ
	unsigned char col_0 = col * CHAR_WIDTH;  				// ����λ
	rstCS
	LCD_TransferCmd(0xb0 + page);		//ѡ��ҳ��
	LCD_TransferCmd(0x10+(col_1 & 0x0f));		//���á��С��Լ��еĸ���λ
	LCD_TransferCmd(0x00+(col_0 & 0x0f));		//���á��С��Լ��еĵ���λ
	for (j = 0; j < NUM_CHAR; j++) {
			LCD_TransferData(0x00);		//��������0x00����ʾ0x00
		}
}

/******************************************************************************
 * @brief  	��Һ����8*8��ģ������˳ʱ��90�ȵ���ת��������Һ������ʾģʽ
 * @param	index	��Ҫ��ʾ�ĺ��ֵ���ģ���
 * @param	row	�ַ���ʾ������ ,ȡֵ��Χ0~3
 * @param	col	�ַ���ʾ������ ,ȡֵ��Χ0~119
 * @return  0 �� ��ʾ�����������
 *			1�� ��ʾ��ʾ�ɹ�
 ******************************************************************************/
unsigned char LCD_Draw_Chinese(unsigned char index, unsigned char row,
		unsigned char col) {
	unsigned char * chinese = CHINESE_MATRIX + 32 * index;
	unsigned int i, j;					//������
	unsigned char col_0 = col;		//�е���λ
	unsigned char col_1 = col >> 4;  //�и���λ

	//----------------������-------------
	//-----------����0��ʾ�������------------
	if (col > 111) {
		return 0;
	}
	if (row > 3) {
		return 0;
	}

	for (i = 0; i < 2; i++) {
		rstCS
		LCD_TransferCmd(0xb0 + row);             //���á�ҳ��
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			 //���á��С��Լ��еĸ���λ
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			 //���á��С��Լ��еĵ���λ
		for (j = 0; j < 16; j++) {
			LCD_TransferData(*chinese);
			chinese++;
		}
	}
	return 1; //����1��ʾִ�гɹ���
}

/************************************************************************
 * ��ʼ��ADC��ȡ���ֵ�ѹֵ
 * 		����������������
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

		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ��ʱԼ10ms��������������
		while (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7) == 0x00)
			; 		// �ȴ�KEY̧��
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ��ʱԼ10ms�������ɼ�����

		// 	do something

		return 0x01;
	}

	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == 0x00) {
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ��ʱԼ10ms��������������
		while (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_6) == 0x00)
			; 		// �ȴ�KEY̧��
		SysCtlDelay(10 * (SysCtlClockGet() / 3000)); 		// ��ʱԼ10ms�������ɼ�����

		// 	do something

		return 0x02;
	}

//	if (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7) == 0x00)
//	{
//		SysCtlDelay(10*(SysCtlClockGet() / 3000)); 					// ��ʱԼ10ms��������������
//		while (GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7) == 0x00); 		// �ȴ�KEY̧��
//		SysCtlDelay(10*(SysCtlClockGet() / 3000)); 					// ��ʱԼ10ms�������ɼ�����
//		return 0x03;
//	}

	return 0;
}


void screenpage(char page,unsigned char *p,int l)
{		int j;
	rstCS
	LCD_TransferCmd(0xb0 + page);		//ѡ��ҳ��
	LCD_TransferCmd(0x10+(0 & 0x0f));		//���á��С��Լ��еĸ���λ
	LCD_TransferCmd(0x00+(0 & 0x0f));		//���á��С��Լ��еĵ���λ
	for (j = 0; j < l; j++) {
			LCD_TransferData(p[j]);		//�������ݣ���ʾ����
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
		LCD_TransferCmd(0xb0 + row * 2 + i);            //??�?�
		LCD_TransferCmd(0x10 + (col_1 & 0x0f));			//??�?�???????
		LCD_TransferCmd(0x00 + (col_0 & 0x0f));			//??�?�???????
		for (j = 0; j < 8; j++)
		{
			LCD_TransferData(*add);
			add++;
		}
	}

	return 1; 								//??1??????!
}
