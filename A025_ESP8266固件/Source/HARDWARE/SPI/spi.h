/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: SPI��д
@Function List:
@History    : 
<author> <time> <version > <desc>
����ԭ��
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 				  	    													  
void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

