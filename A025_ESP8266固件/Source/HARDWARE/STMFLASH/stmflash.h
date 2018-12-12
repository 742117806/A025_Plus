/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: stm flash��д
@Function List:
@History    : 
<author> <time> <version > <desc>
����ԭ��
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __STMFLASH_H__
#define __STMFLASH_H__
#include "sys.h"  

//////////////////////////////////////////////////////////////////////////////////////////////////////
//�û������Լ�����Ҫ����
#define STM32_FLASH_SIZE 64 	 		//��ѡSTM32��FLASH������С(��λΪK)
#define STM32_FLASH_WREN 1              //ʹ��FLASHд��(0��������;1��ʹ��)
//////////////////////////////////////////////////////////////////////////////////////////////////////

//FLASH��ʼ��ַ
#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH����ʼ��ַ
//FLASH������ֵ
 

u16 STMFLASH_ReadHalfWord(u32 faddr);		  //��������  
//void STMFLASH_WriteLenByte(u32 WriteAddr,u32 DataToWrite,u16 Len);	//ָ����ַ��ʼд��ָ�����ȵ�����
//u32 STMFLASH_ReadLenByte(u32 ReadAddr,u16 Len);						//ָ����ַ��ʼ��ȡָ����������

void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead);   		//��ָ����ַ��ʼ����ָ�����ȵ�����

/*------------------------------------------------------------------------
�ر�ע�⣺
	flashд��/��������16λ���� ��һ��2�ֽڣ�����sizeofȡ�õ����ֽڳ��ȣ�
	����Ҫ����2,ת�а��ֵĳ��ȣ�����������ݣ���ɴ��󣡣�����
-------------------------------------------------------------------------*/
#define STM_FLASH_WRITE(X,Y,Z)	STMFLASH_Write(X,Y,Z/2)
#define STM_FLASH_READ(X,Y,Z)	STMFLASH_Read(X,Y,Z/2)

//����д��
void Test_Write(u32 WriteAddr,u16 WriteData);								   
#endif

















