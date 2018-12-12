/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: iap.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: IAP����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  

typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

#define FLASH_APP1_ADDR		0x08002800	   //��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)
										
void iap_load_app(u32 appxaddr);			//ִ��flash�����app����
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin

#endif

/*-------- end of file--------*/


