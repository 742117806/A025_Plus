/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.c
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 串口升级处理操作,STM32F10X   将串口数据保存到外挂的SPI flash中 
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
调试说明：
1、使用RTX_Kenerl RTOS时，使用SPI_Flash_Write函数会出现HardFault错误，原因是SPI_Flash_Write中使用了4096 byte的内存数组，
出现数组溢出错误，修改为SPI_Flash_Write_NoCheck来写入数据。注意：必须要先统一擦除扇区
*/

extern u16 oledFrameSN; //上一个帧序

//把串口数据存到FLASH中
u8 SaveFontToFlash(void)
{
	u32 cheksum; //校验和
	u16 i;
	UartFrame* pFrame;
	u32 page_addr;  //扇区页地址
//	u16 appflag;
	u8 respon; //应答

	pFrame = (UartFrame*)&USART_RX_BUF;  

	//计算校验和
	cheksum = 0;
	for(i=0;i<DATA_SIZE;i++)
	{
		cheksum += pFrame->data_buf[i];
	}

	//判断校验和是否正确
	if (cheksum != pFrame->check_sum)//校验和不对		
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	//判断帧数
	if (pFrame->frame_sn == CMD_FRAME_RESET) //接收复位
	{
		//清除扇区  
		//擦除一个山区的最少时间:150ms
		//每个扇区大小为4096 字节 即4K每扇区
		//STM32 Flash最大容量是128K, 128/4 = 32个扇区
		oledFrameSN =0;

		for(i=0;i<=FLASH_FONT_MAX_SECTOR;i++)
		{
			SPI_Flash_Erase_Sector(i+FLASH_FONT_BEGIN_SECTOR);
		}
		//SPI_Flash_Erase_Sector(FLASH_FONT_BEGIN_SECTOR+FLASH_FONT_MAX_SECTOR+1); //清除SPI_APP_FLAG标记所在的扇区: 

		respon = RET_OK;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 0;
	}


	if (pFrame->frame_sn == 1)  //第一帧数据
	{
		oledFrameSN = 1;
	}
	else if ((oledFrameSN+1 == pFrame->frame_sn) || (pFrame->frame_sn == CMD_FRAME_END))//判断接收的帧是否连续,或是最后一帧
	{
		oledFrameSN += 1;
	}
	else
	{
		//帧序不对，返回错误
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	//写到FLASH中
	page_addr =FLASH_FONT_START_SECTOR + (oledFrameSN-1)*DATA_SIZE; //计算地址

	//判断数据有没有超出128K容量,防止错误传输破坏数据
	if (page_addr >= FLASH_FONT_START_SECTOR+(u32)FLASH_SECTOR_SIZE * FLASH_FONT_MAX_SECTOR)
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	if (pFrame->frame_sn == CMD_FRAME_END) //最后一帧
	{
		SetClockFun(Desktop);
		SetUartRxType(USART_TYPE_LED);  //切换到正常的LED指令
	}

	SPI_Flash_WrData((u8*)USART_RX_BUF,page_addr,DATA_SIZE);//保存到SPI FLASH中
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
	return 0;
}

/*----------------end of file-------------*/

