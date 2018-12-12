/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: OLED.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 显示开机信息 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __DISPLAY_H
#define __DISPLAY_H
#include "sys.h"

void initMessage(void);  //显示开机信息
void OLED_MoveImg128X32(const u8 *pic);
void ShowImg128X32(const u8 *pic);
void ErasureImg128X32(void);
#endif

