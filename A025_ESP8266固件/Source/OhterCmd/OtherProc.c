/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 其它增加的串口处理指令
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "OtherProc.h"
#include "CmdProc.h"
#include "usart.h"
#include "func.h"

extern u8 respon; //回应数据
extern u8 CPU_GUID[12]; //96位芯片唯一标识

void GetChipeInfo(void);

//此函数会被串口接收命令调用,根据需要在这里解析串口指令
//自己写的函数注意不能使用大数组，避免减少使用静态变量，函数执行时间要尽可能短。
//指令中参数值尽量转换成0~100的值,至少不能出现大于0xC0的值，避免与指令冲突

void OtherProc(void)
{
	switch(pOhterCmd->mode) //判断是获取还是设置
	{
	case FRAME_MODE_GET:
		//这里添加获取信息的命令
		GetChipeInfo();
		break;
	case FRAME_MODE_SET:
		//这里添加设置信息的命令
		break;
	}
}

//示例:取GUID和版本
void GetChipeInfo(void)
{
	u8 i;
	STRUCT_OTHER_CMD cmd;

	ClearRam((u8*)&cmd,sizeof(cmd)); //把命令重置为0,防止内存为随机数,出现数据与指令冲突的情况

	cmd.start = FRAME_START;
	cmd.type = FRAME_TYPE_OTHTER;
	cmd.mode = FRAME_MODE_GET;
	cmd.end = FRAME_END;

	cmd.myCmd = 0;//因为没有定义指令，所以用0填充

	//para[0]~para[11]是GUID,也可根据需要取0~11 byte以内的数据
	for(i=0;i<12;i++)
	{
		cmd.para[i]=CPU_GUID[i];
	}
	
	//para[12] 是版本信息
	cmd.para[12] = 0x12; 
	
	//para[13]~para[14] 保留未用

	uart_sendbuf((u8*)&cmd,sizeof(cmd)); //发送数据
}


//------------end of file----------------

