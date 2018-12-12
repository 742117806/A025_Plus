/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 菜单显示
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef _MENU_H_
#define _MENU_H_
#include "key.h"

#define	LCD1602MaxDisplaYLine	2	//LCD最大显示行数
#define ShowString(X,Y,Z)	 OLED_ShowString((X)*8,((Y)*16),(const u8*)Z)
#define ShowStringCN(X,Y,Z)	 OLED_ShowStringCn((X)*8,Y,Z)

typedef struct _subChoose
{
	u8 step;
	u8 maxSetp;
}SubChoose;

//MenuFlag 记录按键在菜单界面和函数相关操作
typedef union _menuflag
{
	u8 Byte;
	struct _m
	{
		u8 isMenu : 1; //0：按键操作在菜单界面,1:按键操作在功能界面
		u8 runFunc : 1; //0:不执行，1：执行函数功能
		u8 updataMenu : 1; //0:不执行，1：更新菜单
		u8 isInMenu : 1; //0:首次进入菜单
		u8 n0 : 4;	//
	}Bits;
}MenuFlag;

extern MenuFlag menuFlag;
void ResetMenuToDestop(void);

#endif

