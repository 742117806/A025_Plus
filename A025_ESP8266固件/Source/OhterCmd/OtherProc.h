/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: LED指令解析
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef H_CMD_OTHER_PROC_H
#define H_CMD_OTHER_PROC_H

#include "sys.h"
void OtherProc(void);

//其它命令结构体
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 myCmd; //这里可以定义自己的二级命令
	u8 para[15]; //这15个字节是命令数据
	u8 end;
}STRUCT_OTHER_CMD;

#define  pOhterCmd		((STRUCT_OTHER_CMD *)TX_RX_BUF) //

#endif

