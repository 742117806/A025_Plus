/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 串口升级处理操作,STM32F10X   将串口数据保存到外挂的SPI flash中 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __IAP_PROC_H__
#define __IAP_PROC_H__
#include "sys.h"  

//注意W25Q64 每个扇区是4K

#define DATA_SIZE		512	//1024   //数据大小


//数据帧结构
typedef struct _UartFrame
{
	u8	data_buf[DATA_SIZE];  //一次传一个扇区的数据
	u32 frame_sn;   //帧序
	u32	check_sum;  //校验和
}UartFrame;

#define CMD_ERROR	1
#define RET_OK		245		//0xF5
#define RET_NG		246		//0xF6

#define CMD_FRAME_END	0x0000FEFE //最后一帧
#define CMD_FRAME_RESET	0x0000FDFD //复位帧

//#define CMD_UPDATA_APP	0x0000FEFD //升级字库


/***********************************
W25Q64 FLASH空间划分: 
1、第0至第32扇区，存储APP程序。[STM32的容量为128K，最大占用32个扇区]
2、第33扇区，存储APP数据标志
***********************************/

#define FLASH_SIZE	0x800000 //8*1024*1024;	//FLASH 大小为8M字节
#define FLASH_SECTOR_SIZE	4096	//每个扇区为4K字节

//1、第0至第32扇区，存储APP程序
#define FLASH_APP_START_SECTOR	0x0000  //开始地址
#define FLASH_APP_MAX_SECTOR	31	//最多占用32个扇区

//2、第33扇区，存储APP数据标志
#define FLASH_FLAG_OK	0x5AA5    //FLAHS数据完成标志	
#define FLASH_FLAG_NG	0xFFFF    //FLAHS数据清除标志	
//#define FLASH_ADDR_FLAG	0x20000  //32*4096 = 0x20000
#define SPI_APP_FLAG	((u32)FLASH_SECTOR_SIZE*FLASH_APP_MAX_SECTOR +0)// APP数据传输成功
#define SPI_APP_LEN		((u32)FLASH_SECTOR_SIZE*FLASH_APP_MAX_SECTOR +16)//APP数据长度

#define SPI_Flash_WrData(X,Y,Z)		SPI_Flash_Write_NoCheck(X,Y,Z)  //写入数据

u8 SaveDataToFlash(void); //将串口数据写到FLASH中
void UpdataIapFlash(void);  //执行IAP更新操作
void SoftReset(void); //复位重启
#endif

/*----------end of file------*/





