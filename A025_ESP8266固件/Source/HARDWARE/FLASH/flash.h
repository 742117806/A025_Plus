/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: flash.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: SPI�ӿ�flash��д����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __FLASH_H
#define __FLASH_H			    
#include "sys.h" 
	  
//W25Xϵ��/Qϵ��оƬ�б�	   
//W25Q80 ID  0XEF13
//W25Q16 ID  0XEF14
//W25Q32 ID  0XEF15
//W25Q32 ID  0XEF16	
#define W25Q80 	0XEF13 	
#define W25Q16 	0XEF14
#define W25Q32 	0XEF15
#define W25Q64 	0XEF16

extern u16 SPI_FLASH_TYPE;//��������ʹ�õ�flashоƬ�ͺ�

#define	SPI_FLASH_CS PBout(12)  //ѡ��FLASH	 
 
void SPI_Flash_Init(void);
u16  SPI_Flash_ReadID(void);  	    //��ȡFLASH ID
u8	 SPI_Flash_ReadSR(void);        //��ȡ״̬�Ĵ��� 
void SPI_FLASH_Write_SR(u8 sr);  	//д״̬�Ĵ���
void SPI_FLASH_Write_Enable(void);  //дʹ�� 
void SPI_FLASH_Write_Disable(void);	//д����
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void SPI_Flash_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void SPI_Flash_Erase_Chip(void);    	  //��Ƭ����
void SPI_Flash_Erase_Sector(u32 Dst_Addr);//��������
void SPI_Flash_Wait_Busy(void);           //�ȴ�����
void SPI_Flash_PowerDown(void);           //�������ģʽ
void SPI_Flash_WAKEUP(void);			  //����
#endif

/*-------------------end of file------------------*/


