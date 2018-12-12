/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: led.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: LED指示灯驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0	PAout(15)// PB5 15
#define LED_WIFI	PBout(3)// 
#define LED0_ON		(LED0=1)
#define LED0_OFF	(LED0=0)

#define LED_WIFI_ON		(LED_WIFI=1)
#define LED_WIFI_OFF	(LED_WIFI=0)

	

void LED_Init(void);//初始化
		 				    
#endif

/*---------end of file----------*/
