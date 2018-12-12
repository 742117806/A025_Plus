/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.c
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ���������������,STM32F10X   ���������ݱ��浽��ҵ�SPI flash�� 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "sys.h"
#include "updataFont.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "stm32f10x_flash.h"
#include "flash.h"
#include "iap.h"

#include "func.h"
#include "ESP8266_07S.h"
/*
����˵����
1��ʹ��RTX_Kenerl RTOSʱ��ʹ��SPI_Flash_Write���������HardFault����ԭ����SPI_Flash_Write��ʹ����4096 byte���ڴ����飬
����������������޸�ΪSPI_Flash_Write_NoCheck��д�����ݡ�ע�⣺����Ҫ��ͳһ��������
*/

extern u16 oledFrameSN; //��һ��֡��

//�Ѵ������ݴ浽FLASH��
u8 SaveFontToFlash(void)
{
	u32 cheksum; //У���
	u16 i;
	UartFrame* pFrame;
	u32 page_addr;  //����ҳ��ַ
//	u16 appflag;
	u8 respon; //Ӧ��

	pFrame = (UartFrame*)&USART_RX_BUF;  

	//����У���
	cheksum = 0;
	for(i=0;i<DATA_SIZE;i++)
	{
		cheksum += pFrame->data_buf[i];
	}

	//�ж�У����Ƿ���ȷ
	if (cheksum != pFrame->check_sum)//У��Ͳ���		
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	//�ж�֡��
	if (pFrame->frame_sn == CMD_FRAME_RESET) //���ո�λ
	{
		//�������  
		//����һ��ɽ��������ʱ��:150ms
		//ÿ��������СΪ4096 �ֽ� ��4Kÿ����
		//STM32 Flash���������128K, 128/4 = 32������
		oledFrameSN =0;

		for(i=0;i<=FLASH_FONT_MAX_SECTOR;i++)
		{
			SPI_Flash_Erase_Sector(i+FLASH_FONT_BEGIN_SECTOR);
		}
		//SPI_Flash_Erase_Sector(FLASH_FONT_BEGIN_SECTOR+FLASH_FONT_MAX_SECTOR+1); //���SPI_APP_FLAG������ڵ�����: 

		respon = RET_OK;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 0;
	}


	if (pFrame->frame_sn == 1)  //��һ֡����
	{
		oledFrameSN = 1;
	}
	else if ((oledFrameSN+1 == pFrame->frame_sn) || (pFrame->frame_sn == CMD_FRAME_END))//�жϽ��յ�֡�Ƿ�����,�������һ֡
	{
		oledFrameSN += 1;
	}
	else
	{
		//֡�򲻶ԣ����ش���
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	//д��FLASH��
	page_addr =FLASH_FONT_START_SECTOR + (oledFrameSN-1)*DATA_SIZE; //�����ַ

	//�ж�������û�г���128K����,��ֹ�������ƻ�����
	if (page_addr >= FLASH_FONT_START_SECTOR+(u32)FLASH_SECTOR_SIZE * FLASH_FONT_MAX_SECTOR)
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	if (pFrame->frame_sn == CMD_FRAME_END) //���һ֡
	{
		SetClockFun(Desktop);
		SetUartRxType(USART_TYPE_LED);  //�л���������LEDָ��
	}

	SPI_Flash_WrData((u8*)USART_RX_BUF,page_addr,DATA_SIZE);//���浽SPI FLASH��
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
	return 0;
}

/*----------------end of file-------------*/

