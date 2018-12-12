/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ���������������,STM32F10X   ���������ݱ��浽��ҵ�SPI flash�� 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __IAP_PROC_H__
#define __IAP_PROC_H__
#include "sys.h"  

//ע��W25Q64 ÿ��������4K

#define DATA_SIZE		512	//1024   //���ݴ�С


//����֡�ṹ
typedef struct _UartFrame
{
	u8	data_buf[DATA_SIZE];  //һ�δ�һ������������
	u32 frame_sn;   //֡��
	u32	check_sum;  //У���
}UartFrame;

#define CMD_ERROR	1
#define RET_OK		245		//0xF5
#define RET_NG		246		//0xF6

#define CMD_FRAME_END	0x0000FEFE //���һ֡
#define CMD_FRAME_RESET	0x0000FDFD //��λ֡

//#define CMD_UPDATA_APP	0x0000FEFD //�����ֿ�


/***********************************
W25Q64 FLASH�ռ仮��: 
1����0����32�������洢APP����[STM32������Ϊ128K�����ռ��32������]
2����33�������洢APP���ݱ�־
***********************************/

#define FLASH_SIZE	0x800000 //8*1024*1024;	//FLASH ��СΪ8M�ֽ�
#define FLASH_SECTOR_SIZE	4096	//ÿ������Ϊ4K�ֽ�

//1����0����32�������洢APP����
#define FLASH_APP_START_SECTOR	0x0000  //��ʼ��ַ
#define FLASH_APP_MAX_SECTOR	31	//���ռ��32������

//2����33�������洢APP���ݱ�־
#define FLASH_FLAG_OK	0x5AA5    //FLAHS������ɱ�־	
#define FLASH_FLAG_NG	0xFFFF    //FLAHS���������־	
//#define FLASH_ADDR_FLAG	0x20000  //32*4096 = 0x20000
#define SPI_APP_FLAG	((u32)FLASH_SECTOR_SIZE*FLASH_APP_MAX_SECTOR +0)// APP���ݴ���ɹ�
#define SPI_APP_LEN		((u32)FLASH_SECTOR_SIZE*FLASH_APP_MAX_SECTOR +16)//APP���ݳ���

#define SPI_Flash_WrData(X,Y,Z)		SPI_Flash_Write_NoCheck(X,Y,Z)  //д������

u8 SaveDataToFlash(void); //����������д��FLASH��
void UpdataIapFlash(void);  //ִ��IAP���²���
void SoftReset(void); //��λ����
#endif

/*----------end of file------*/





