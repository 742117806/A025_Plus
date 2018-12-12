/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: func.h
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ��ʱ����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/
#include "TimerFunc.h"
#include "func.h"
#include "rtc.h"
#include "system.h"
#include "stmflash.h"
#include "my_type.h"
#include "timer.h"
#include "rtx_os.h"

u8 GetMaxTimerStep(void);

//���㵱ǰ������
//ʵ������ʱ���֮�佥���Ч��
//�㷨�����㵱ǰʱ��������ʱ���֮ǰ�İٷֱȣ�ͨ��ʱ��ٷֱȼ�������ֵ��
void SetPwmByTimer(u8 hour,u8 minute)
{
	u16 t1,t2,t_3,t_total; //t1ʱ���1;t2ʱ���2;t3��t2��t1��ʱ���
	u16 tc;  //��ǰʱ��
	float percent; //�ٷֱ�
	u8 t_pwm;
	u8 c_pwm;

	u8 pwmX[PWM_NUM];
	u8 *p1,*p2;
	u8 i;

	sysInfo.indexTimer = findCurrectPwm(hour,minute); //�ҵ���ǰ��ʱ��
	//sysInfo.nextTimer = (sysInfo.indexTimer +1)%PRGM_STEP; //��һ��ʱ���
	sysInfo.nextTimer = (sysInfo.indexTimer +1)% GetMaxTimerStep(); //��һ��ʱ���

	t1 = TimerProg[sysInfo.indexTimer ].hour*60+TimerProg[sysInfo.indexTimer].minute; //ʱ���1
	t2 = TimerProg[sysInfo.nextTimer].hour*60+TimerProg[sysInfo.nextTimer].minute; //ʱ���2
	tc = hour*60+minute; //��ǰʱ��
	
	if (tc < t1)tc += 1440; //���˵ڶ���

	if (t2 < t1)t2 += 1440; //t2��t1С��ѭ�����ڶ��� 24*60=1440��;
			
	t_3 = tc - t1; //��ǰʱ�䵽��ʱ1�ĳ���

	t_total = t2 - t1; //������ʱ��ĳ���
	percent = (float)t_3 / (float)t_total;//���㵱ǰʱ������İٷֱ�

	//����PWM1~PWM6��ֵ
	p1 = &TimerProg[sysInfo.indexTimer].pwm1;
	p2 = &TimerProg[sysInfo.nextTimer].pwm1;
	for (i=0;i<PWM_NUM;i++) 
	{
		if (*p2 >= *p1)
		{
			t_pwm = *p2 - *p1;
			c_pwm = *p1+ (char)(percent*t_pwm);
		}
		else
		{
			t_pwm = *p1 - *p2;
			c_pwm = *p1- (char)(percent*t_pwm);
		}
		pwmX[i] = c_pwm;
		p1++;
		p2++;
	}

	//�õ�ǰ������
	SetPwm(
		pwmX[0],
		pwmX[1],
		pwmX[2],
		pwmX[3],
		pwmX[4],
		pwmX[5]
		);
}

/********************************************************************************** 
 * Function Name  : void findCurrectPwm(void)
 * Description    : ���ҵ�ǰʱ��PWMԤ��ֵ,ʱ��ѭ����ʽ lfx
 * Input&
 * Output   : ���ص�ǰ�Ķ�ʱ���
 ********************************************************************************/
u8 findCurrectPwm(u8 hour,u8 minute)
{
	u16  timeNow;	//��ǰʱ��
	u16	 timePwm;
	u8  i,post;	
	BOOL IsFind = FALSE;	//�ҵ��ȵ�ǰʱ��С�ı�־

	timeNow = hour*HOUR_RADIX+minute; //���㵱ǰʱ��
	STM_FLASH_READ(FLASH_TIMER_ADDR,(u16*)TimerProg,sizeof(TimerProg));//��ȡPWM��Ϣ

	//���㵱ǰʱ����������ʱ���е�λ��
	IsFind = FALSE;  
	for(i=0;i<PRGM_STEP;i++)
	{
		if (TimerProg[i].enable)// != DISABLE
		{
			timePwm = TimerProg[i].hour*HOUR_RADIX + TimerProg[i].minute ; //����ʱ��
			if((timeNow > timePwm))//ʱ����Ȳ���ҪΪ����״̬
			{
				IsFind = TRUE;
				post = i;	//��¼λ��,�����ҵ�������λ��
			}
		}
		else
		{
			break; //����ѭ��
		}
	}

	if(!IsFind)//û���ҵ�
	{
	//	post = PRGM_STEP - 1;	//ȡ���һ��ʱ�������� 
		post = GetMaxTimerStep()-1;  //ע�⣺��ʱʱ�αضδ��ڵ���1

	}
	return post;
}

//ȡ���Ķ�ʱ��
u8 GetMaxTimerStep(void)
{
	u8 setp;
	u8 i;

	setp = 0;
	for (i=0;i<PRGM_STEP;i++)
	{
		if (TimerProg[i].enable) //��ʱ�������ͼ�1
		{
			setp++;
		}
		else  
		{
			break;  //����ѭ��
		}
	}
	
	return setp;
}

//�ڴ渴��
//source Դ��ַ
//target Ŀ���ַ
void TimerCopy(TIMER* target,TIMER* source)
{
	u8 size = sizeof(TIMER);
	u8* ps = (u8*)source;
	u8* pt = (u8*)target;
	while(size--)
	{
		*pt = *ps;
		pt++;
		ps++;
	}
}
/*---------------lfx---------------*/
//����ʱʱ��
void SortTimer(void)
{
	u8 i,j;//m;
	u16 time1,time2;
	TIMER temp;

	for(i=PRGM_STEP-1;i!=0;i--)
	{
		for (j = 0;j < i; j++)
		{
			time1 = TimerProg[j].hour * HOUR_RADIX + TimerProg[j].minute;
			time2 = TimerProg[j+1].hour * HOUR_RADIX + TimerProg[j+1].minute;
			if(time1 > time2)
			{
				//��������
				TimerCopy(&temp,&TimerProg[j]);
				TimerCopy(&TimerProg[j],&TimerProg[j+1]);
				TimerCopy(&TimerProg[j+1],&temp);
			}
		}
	}
}


//-----------end of file--------------

