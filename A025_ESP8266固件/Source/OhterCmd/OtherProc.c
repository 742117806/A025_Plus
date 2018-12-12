/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �������ӵĴ��ڴ���ָ��
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "OtherProc.h"
#include "CmdProc.h"
#include "usart.h"
#include "func.h"

extern u8 respon; //��Ӧ����
extern u8 CPU_GUID[12]; //96λоƬΨһ��ʶ

void GetChipeInfo(void);

//�˺����ᱻ���ڽ����������,������Ҫ�������������ָ��
//�Լ�д�ĺ���ע�ⲻ��ʹ�ô����飬�������ʹ�þ�̬����������ִ��ʱ��Ҫ�����̡ܶ�
//ָ���в���ֵ����ת����0~100��ֵ,���ٲ��ܳ��ִ���0xC0��ֵ��������ָ���ͻ

void OtherProc(void)
{
	switch(pOhterCmd->mode) //�ж��ǻ�ȡ��������
	{
	case FRAME_MODE_GET:
		//������ӻ�ȡ��Ϣ������
		GetChipeInfo();
		break;
	case FRAME_MODE_SET:
		//�������������Ϣ������
		break;
	}
}

//ʾ��:ȡGUID�Ͱ汾
void GetChipeInfo(void)
{
	u8 i;
	STRUCT_OTHER_CMD cmd;

	ClearRam((u8*)&cmd,sizeof(cmd)); //����������Ϊ0,��ֹ�ڴ�Ϊ�����,����������ָ���ͻ�����

	cmd.start = FRAME_START;
	cmd.type = FRAME_TYPE_OTHTER;
	cmd.mode = FRAME_MODE_GET;
	cmd.end = FRAME_END;

	cmd.myCmd = 0;//��Ϊû�ж���ָ�������0���

	//para[0]~para[11]��GUID,Ҳ�ɸ�����Ҫȡ0~11 byte���ڵ�����
	for(i=0;i<12;i++)
	{
		cmd.para[i]=CPU_GUID[i];
	}
	
	//para[12] �ǰ汾��Ϣ
	cmd.para[12] = 0x12; 
	
	//para[13]~para[14] ����δ��

	uart_sendbuf((u8*)&cmd,sizeof(cmd)); //��������
}


//------------end of file----------------

