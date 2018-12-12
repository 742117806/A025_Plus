/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: SPI读写
@Function List:
@History    : 
<author> <time> <version > <desc>
正点原子
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __SPI_H
#define __SPI_H
#include "sys.h"
 				  	    													  
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
		 
#endif

