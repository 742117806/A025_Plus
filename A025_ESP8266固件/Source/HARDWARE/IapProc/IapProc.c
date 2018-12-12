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
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "stm32f10x_flash.h"
#include "flash.h"
#include "iap.h"
#include "ESP8266_07S.h"
/*
调试说明：
1、使用RTX_Kenerl RTOS时，使用SPI_Flash_Write函数会出现HardFault错误，原因是SPI_Flash_Write中使用了4096 byte的内存数组，
出现数组溢出错误，修改为SPI_Flash_Write_NoCheck来写入数据。注意：必须要先统一擦除扇区
*/
u16 oledFrameSN = 0; //上一个帧序

//把串口数据存到FLASH中
u8 SaveDataToFlash(void)
{
	u32 cheksum; //校验和
	u16 i;
	UartFrame* pFrame;
	u32 page_addr;  //扇区页地址
	u16 appflag;
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
		/*
		oledFrameSN = 0;
		appflag = FLASH_FLAG_NG;
		SPI_Flash_Write((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //写上标记
		SPI_Flash_Write((u8*)&oledFrameSN,SPI_APP_LEN,sizeof(oledFrameSN)); //代码长度
		*/

		//清除扇区  
		//擦除一个山区的最少时间:150ms
		//每个扇区大小为4096 字节 即4K每扇区
		//STM32 Flash最大容量是128K, 128/4 = 32个扇区
		
		for(i=0;i<=FLASH_APP_MAX_SECTOR;i++)
		{
			SPI_Flash_Erase_Sector(i);
		}
	//	SPI_Flash_Erase_Sector(FLASH_APP_MAX_SECTOR+1); //清除SPI_APP_FLAG标记所在的扇区: 

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
	page_addr = /*SPI_FLASH_ADDR + */(oledFrameSN-1)*DATA_SIZE; //计算地址

	//判断数据有没有超出128K容量,防止错误传输破坏数据
	if (page_addr >= (u32)FLASH_SECTOR_SIZE * (FLASH_APP_MAX_SECTOR-1))
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		return 1;
	}

	SPI_Flash_WrData((u8*)USART_RX_BUF,page_addr,DATA_SIZE);//保存到SPI FLASH中
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);

	if (pFrame->frame_sn == CMD_FRAME_END) //最后一帧
	{
		appflag = FLASH_FLAG_OK;
		SPI_Flash_WrData((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //写上标记
		SPI_Flash_WrData((u8*)&oledFrameSN,SPI_APP_LEN,sizeof(oledFrameSN)); //代码长度
		//UpdataIapFlash();
		delay_ms(1000); //延时一下,等待WIFI数据发送完成

		SoftReset();  //重启,执行bootload来更新IAP
	}
	return 0;
}


//读取FLASH，并从串口输出
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
//更新IAP
void UpdataIapFlash(void)
{
	u16 appflag;
	u16 applen;
	u16 addr;
	u32 iapaddr;

	SPI_Flash_Read((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag)); //读标记

	if (appflag == FLASH_FLAG_OK)
	{
		SPI_Flash_Read((u8*)&applen,SPI_APP_LEN,sizeof(applen));
		for(addr=0;addr < applen;addr++)  //更新IAP
		{
			SPI_Flash_Read(USART_RX_BUF,addr*1024,1024);
			iapaddr = FLASH_APP1_ADDR+addr*1024;
			iap_write_appbin(iapaddr,USART_RX_BUF,1024);//更新FLASH代码 
		}

		//清除标记
		appflag = 0xFFFF;
		SPI_Flash_WrData((u8*)&appflag,SPI_APP_FLAG,sizeof(appflag));
		applen = 0;
		SPI_Flash_WrData((u8*)&applen,SPI_APP_LEN,sizeof(applen));

		iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
	}
}

//软件复位
void SoftReset()
{
	INTX_DISABLE();  //关闭所有中断
	NVIC_SystemReset();
}

/*----------------end of file-------------*/

