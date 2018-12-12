/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iapProc.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �����ֿ⵽SPI Flash��
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

//���ļ�������iapProc�ļ�

#ifndef __IAP_FONT_H__
#define __IAP_FONT_H__
#include "sys.h"  
#include "IapProc.h"


//1����0����32�������洢APP����
#define FLASH_FONT_START_SECTOR	0x100000  //��ʼ��ַ
#define FLASH_FONT_MAX_SECTOR	256	//��ռ��256������ 1M�ֽ�
#define FLASH_FONT_BEGIN_SECTOR	256

u8 SaveFontToFlash(void); //����������д��FLASH��

#endif

/*----------end of file------*/





