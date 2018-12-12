/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 升级字库到SPI Flash中
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

//此文件依赖于iapProc文件

#ifndef __IAP_FONT_H__
#define __IAP_FONT_H__
#include "sys.h"  
#include "IapProc.h"


//1、第0至第32扇区，存储APP程序
#define FLASH_FONT_START_SECTOR	0x100000  //开始地址
#define FLASH_FONT_MAX_SECTOR	256	//最占用256个扇区 1M字节
#define FLASH_FONT_BEGIN_SECTOR	256

u8 SaveFontToFlash(void); //将串口数据写到FLASH中

#endif

/*----------end of file------*/





