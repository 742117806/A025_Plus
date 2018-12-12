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
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "stm32f10x_flash.h"
#include "flash.h"
#include "iap.h"
#include "ESP8266_07S.h"
/*
����˵����
1��ʹ��RTX_Kenerl RTOSʱ��ʹ��SPI_Flash_Write���������HardFault����ԭ����SPI_Flash_Write��ʹ����4096 byte���ڴ����飬
����������������޸�ΪSPI_Flash_Write_NoCheck��д�����ݡ�ע�⣺����Ҫ��ͳһ��������
*/
u16 oledFrameSN = 0; //��һ��֡��

//�Ѵ������ݴ浽FLASH��
u8 SaveDataToFlash(void)
{
	u32 cheksum; //У���
	u16 i;
	UartFrame* pFrame;
	u32 page_addr;  //����ҳ��ַ
	u16 appflag;
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
		/*
		oledFrameSN = 0;
		appflag = FLASH_FLAG_NG;
		SPI_Flash_Write((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //д�ϱ��
		SPI_Flash_Write((u8*)&oledFrameSN,SPI_APP_LEN,sizeof(oledFrameSN)); //���볤��
		*/

		//�������  
		//����һ��ɽ��������ʱ��:150ms
		//ÿ��������СΪ4096 �ֽ� ��4Kÿ����
		//STM32 Flash���������128K, 128/4 = 32������
		
		for(i=0;i<=FLASH_APP_MAX_SECTOR;i++)
		{
			SPI_Flash_Erase_Sector(i);
		}
	//	SPI_Flash_Erase_Sector(FLASH_APP_MAX_SECTOR+1); //���SPI_APP_FLAG������ڵ�����: 

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
	page_addr = /*SPI_FLASH_ADDR + */(oledFrameSN-1)*DATA_SIZE; //�����ַ

	//�ж�������û�г���128K����,��ֹ�������ƻ�����
	if (page_addr >= (u32)FLASH_SECTOR_SIZE * (FLASH_APP_MAX_SECTOR-1))
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	SPI_Flash_WrData((u8*)USART_RX_BUF,page_addr,DATA_SIZE);//���浽SPI FLASH��
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);

	if (pFrame->frame_sn == CMD_FRAME_END) //���һ֡
	{
		appflag = FLASH_FLAG_OK;
		SPI_Flash_WrData((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //д�ϱ��
		SPI_Flash_WrData((u8*)&oledFrameSN,SPI_APP_LEN,sizeof(oledFrameSN)); //���볤��
		//UpdataIapFlash();
		delay_ms(1000); //��ʱһ��,�ȴ�WIFI���ݷ������

		SoftReset();  //����,ִ��bootload������IAP
	}
	return 0;
}


//��ȡFLASH�����Ӵ������
/*
void readflash()
{
	u8 *p;
	u16 addr;
	p = USART_RX_BUF;
	for(addr=0;addr < 5;addr++)
	{
		SPI_Flash_Read(p,addr*1024,1024);
		uart_sendbuf(p,1024);
	}
}
*/
//����IAP
void UpdataIapFlash(void)
{
	u16 appflag;
	u16 applen;
	u16 addr;
	u32 iapaddr;

	SPI_Flash_Read((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //�����

	if (appflag == FLASH_FLAG_OK)
	{
		SPI_Flash_Read((u8*)&applen,SPI_APP_LEN,sizeof(applen));
		for(addr=0;addr < applen;addr++)  //����IAP
		{
			SPI_Flash_Read(USART_RX_BUF,addr*1024,1024);
			iapaddr = FLASH_APP1_ADDR+addr*1024;
			iap_write_appbin(iapaddr,USART_RX_BUF,1024);//����FLASH���� 
		}

		//������
		appflag = 0xFFFF;
		SPI_Flash_WrData((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag));
		applen = 0;
		SPI_Flash_WrData((u8*)&applen,SPI_APP_LEN,sizeof(applen));

		iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
	}
}

//�����λ
void SoftReset()
{
	INTX_DISABLE();  //�ر������ж�
	NVIC_SystemReset();
}

/*----------------end of file-------------*/

