/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: stm flash读写
@Function List:
@History    : 
<author> <time> <version > <desc>
正点原子
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////////////////////////
//用户根据自己的需要设置
#define STM32_FLASH_SIZE 64 	 		//所选STM32的FLASH容量大小(单位为K)
#define STM32_FLASH_WREN 1              //使能FLASH写入(0，不是能;1，使能)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH起始地址
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址
//FLASH解锁键值
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //读出半字  
//void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//指定地址开始写入指定长度的数据
//u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//指定地址开始读取指定长度数据

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//从指定地址开始写入指定长度的数据
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//从指定地址开始读出指定长度的数据

/*------------------------------------------------------------------------
特别注意：
	flash写入/读出是以16位进行 即一次2字节，而用sizeof取得的是字节长度，
	所以要除以2,转有半字的长度，否则会多读数据，造成错误！！！！
-------------------------------------------------------------------------*/
#define STM_FLASH_WRITE(X,Y,Z)	STMFLASH_Write(X,Y,Z/2)
#define STM_FLASH_READ(X,Y,Z)	STMFLASH_Read(X,Y,Z/2)

//测试写入
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















