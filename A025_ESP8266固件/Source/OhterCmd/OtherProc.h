/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: LEDָ�����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#ifndef H_CMD_OTHER_PROC_H
#define H_CMD_OTHER_PROC_H

#include "sys.h"
void OtherProc(void);

//��������ṹ��
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 myCmd; //������Զ����Լ��Ķ�������
	u8 para[15]; //��15���ֽ�����������
	u8 end;
}STRUCT_OTHER_CMD;

#define  pOhterCmd		((STRUCT_OTHER_CMD *)TX_RX_BUF) //

#endif

