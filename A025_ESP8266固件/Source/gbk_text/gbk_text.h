/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: menu_cn.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-10-24
@Description: 汉字显示
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef __TEXT_H__
#define __TEXT_H__	 
#include "sys.h"
					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);//得到汉字的点阵码
void Show_Font(u8 x,u8 y,u8 *font,u8 mode);//在指定位置显示一个汉字
void OLED_ShowStringCn(u8 x,u8 y,u8 *str);
void OLED_DrawFont16X16(u8 Start_X,u8 Start_Y,const u8 *pic);
#endif
