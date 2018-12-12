/*******************************************************
ULN2003����5V���ٲ����������
Target:STC89C52RC-40C
Crystal:12MHz
Author:ս��Ƭ��������
Platform:51&avr��Ƭ����Сϵͳ��+ULN2003������������׼�
http://zsmcu.taobao.com   QQ:284083167
*******************************************************
תһ����Ҫ�Ĳ���:
//���������Ҫ4096������һȦ ����������ֻҪһ�롣 360 / 5.625 / 64 = �����
���߷�ʽ:
IN1 ---- P00
IN2 ---- P01
IN3 ---- P02
IN4 ---- P03
+   ---- +5V
-   ---- GND
*********************/

#include <math.h>
#include "sys.h"
#include "delay.h"
#include "uln2003.h"
#include "rtl.h"
#include "func.h"
#include "rtc.h"


const u8 phasecw[8] = { 0x08, 0x0c, 0x04, 0x06,0x02,0x03,0x01,0x09 };//��ת   �����ͨ���� A-AB-B-BC-C-CD-D-DA
u8 phase_falg = 0;

void TIM1_Int_Init(u16 arr,u16 psc);
//ֹͣת��
void MotorStop(void)
{
	TIM_Cmd(TIM1, DISABLE);
	MotorA = 0;
	MotorB = 0;
	MotorC = 0;
	MotorD = 0;
	
}

void MotorInit(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);        //JTAG-DP ʧ�� + SW-DPʹ�� 


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_6 | GPIO_Pin_8);
	
	TIM1_Int_Init(1300,72-1);
}

//˳ʱ��ת��
//��ת   �����ͨ���� A-AB-B-BC-C-CD-D-DA
void MotorStart(uint8_t flag,u16 arr)
{
	phase_falg = flag;
	TIM1_Int_Init(arr,72-1);
	TIM_Cmd(TIM1, ENABLE);
}

#if 0
//������ת
void MotoTest1(void)
{

}

//������ת
void MotoTest2(void)
{

}

//�������ִ��
void MotorRunPresse(void)
{

	
}
#endif
//���ҵ���ڵ�ǰʱ���Ƿ���Ч
u8 MotorIsEffective(u8 cur_hour, u8 cur_min)
{
	u16  timeNow;	//��ǰʱ��
//	u8 motorSpeed;		//������ִ��һ��
	u16 timeStar, timeEnd;

	timeNow = cur_hour * 60 + cur_min; //��ȡϵͳʱ��


	if (motorInfo.enable == 1)
	{

		//��һ��ʱ���
		timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
		timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
		if (timeEnd >= timeStar)		//��ͬһ��ʱ����
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}
		else		//����ͬһ��ʱ����
		{
	
			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}

		//�ڶ���֮���
		timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
		timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
		if(timeEnd >= timeStar)		//��ͬһ��ʱ����
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}
		else		//����ͬһ��ʱ����
		{

			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}


		//������֮���
		timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
		timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
		if (timeEnd >= timeStar)		//��ͬһ��ʱ����
		{
			if ((timeNow >= timeStar) && (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}
		else		//����ͬһ��ʱ����
		{

			if ((timeNow >= timeStar) || (timeNow < timeEnd))	//��ǰʱ�������࿪ʼ�ͽ���ʱ��֮��
			{
				return 1;
			}
		}
	}
	return 0;
}

//�����ת��ʱ��
uint32_t GetMotorRunTimer(void)
{
	
	u16 timeStar, timeEnd;
	uint32_t timeSum = 0;
	uint32_t current_time;		//��ǰϵͳʱ��

	
	#if 1
	//��һ��ʱ���
	timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	
	//�ڶ���֮���
	timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	
	//������֮���
	timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	if(timeStar<timeEnd)
	{
		timeSum = timeSum + (timeEnd - timeStar);
	}
	timeSum = timeSum * 60;
	
	#else
	current_time = calendar.hour*60+calendar.min;		//��ȡ��ǰ����ʱ��
		//��һ��ʱ���
	timeStar = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	
		//�ڶ���֮���
	timeStar = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	
		//������֮���
	timeStar = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	if((current_time >= timeStar) && (current_time < timeEnd))
	{
		timeSum = timeEnd - timeStar;
	}
	timeSum = timeSum * 60;
	#endif
	return timeSum;
}

//�����ת�ܲ���
uint32_t  GetMotorRunSetp(void)
{
	uint16_t angle;					//�����ת�ܽǶ�
	uint16_t motor_step_sum = 0;	//���Ӧ����ת�Ĳ���
	
	angle = motorInfo.angle_h*100+motorInfo.angle_l;
	motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//��������Ҫ��ת�Ĳ���(�����һ��5��)
	return  motor_step_sum;
}


//��ȡ���ִ��һ�εļ������

//����Զ�����ԭ��
void MotorAutoGoBack(_calendar_obj time)
{
	if((time.hour == 23)&&(time.min == 59)&&(time.sec > 55))
	{
		//SEGGER_RTT_printf(0,"MotorAutoGoBack\r\n");
		MotorGoBack();
	}
}
//�ϵ�ص�ԭ��
void MotorBackOrigin(void)
{

}


void TIM1_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); //��ʱ�� TIM1 ʹ��
	//��ʱ�� TIM1 ��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����ʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM ���ϼ���
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0; //�߼���ʱ��1���ö�ʱ��������������ſ����������ļ���Ƶ��һ��ʼ��72mhz ֵ��ע��ĵط�
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); //�ڳ�ʼ�� TIM1
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE ); //����������ж�
	//�ж����ȼ� NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn; //TIM1 �ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //��ռ���ȼ� 1 ��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ� 3 ��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ ͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure); //�ܳ�ʼ�� NVIC �Ĵ���
	TIM_Cmd(TIM1, ENABLE); //��ʹ�� TIM1

}

static uint8_t index=0;
static u16 tim1_arr = 1500;
u16 motor_current_step = 0;
u16 motor_set_step = 0;
static uint8_t go_back_flag = 0;
static uint8_t step_1;

void TIM1_UP_IRQHandler(void) //TIM1 �ж�
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET) //��� TIM1 �����жϷ������
	{
		TIM_ClearFlag(TIM1, TIM_IT_Update ); //��� TIM1 �����жϱ�־
		if(phase_falg == 0)
		{
			MotorA = (phasecw[index]&0x01?1:0);	
			MotorB = (phasecw[index]&0x02?1:0);	
			MotorC = (phasecw[index]&0x04?1:0);	
			MotorD = (phasecw[index]&0x08?1:0);
			index ++;
			if(index==8)
			{
				index=0;
				if(motor_current_step > 0) 
				{
					motor_current_step--;
				}
				
			}
		}
		else
		{
			MotorD = (phasecw[index]&0x01?1:0);	
			MotorC = (phasecw[index]&0x02?1:0);	
			MotorB = (phasecw[index]&0x04?1:0);	
			MotorA = (phasecw[index]&0x08?1:0);
			index ++;
			if(index==8)
			{
				index=0;
				if(motor_current_step<=LED_MOTOR_STEP_MAX)
				{
					motor_current_step++;
				}
			}
		}	
		if(motor_current_step == motor_set_step)
		{
			MotorStop();
		}
//		index ++;
//		if(index==8)
//		{
//			index=0;
//			motor_current_step++;
//			if(go_back_flag==0)
//			{
//				if(step_1 > motorInfo.speed )			//һ��15��
//				{
//					step_1 = 0;
//					MotorStop();
//				}
//				step_1++;
//			}
//		}
	}
}



//���ٻ�ת
void MotorGoBack(void)
{
	u8 i = 0;
//	go_back_flag = 1;
//	for(i=0;i<15;i++)
//	{
//		MotorStart(0,2000 - (i*100));
//		os_dly_wait(50);
//	}
//	MotorStart(0,1500);
	motor_current_step = LED_MOTOR_STEP_MAX;
	SetMotoCurrentStep(0);
	while(1)
	{
//		if(motor_current_step > step)
//		{
//			MotorStop();
//			go_back_flag = 0;
//			motor_current_step  = 0;
//			break;
//		}
//		SEGGER_RTT_printf(0,"MotorGoBack = %d\r\n",motor_current_step);
		MotorStepRefresh();
		if(motor_current_step == 0)
		{
		   break;
		}
		os_dly_wait(100);
//		os_tsk_pass();
	}
}

//����ǰת
void MotorGoTo(uint16_t step)
{
	go_back_flag = 1;		//����ת�����
	MotorStart(1,1300);
	//MotorStart(1,300);
	while(1)
	{
		if(motor_current_step > step)
		{

			MotorStop();
			go_back_flag = 0;
			break;
		}
		//SEGGER_RTT_printf(0,"MotorGoTo = %d\r\n",motor_current_step);
		os_dly_wait(20);
		os_tsk_pass();
	}
}


//��������趨�ٶ�ת��
//speed:�ٶ�
//ת������
void MotorRun(uint16_t speed,uint8_t dir)
{
	go_back_flag = 1;
	MotorStart(dir,speed);
	
}
//����ߵ���ǰʱ��Ӧ�õ���ĵط�
uint16_t MotorGotoCurrentPoint(uint16_t step_sum,_calendar_obj calendar,MotorInfo_s motor_info)
{
	uint16_t over_time;		//�Ѿ���ȥ��ʱ�䣨��λ�Ƿ��ӣ�
	uint16_t set_time1,set_time2,set_time3;		//����趨��ʱ���
	u16 timeStar1, timeEnd1,timeStar2, timeEnd2,timeStar3, timeEnd3;		//������õ�ʱ�俪ʼ��ʱ�����
	u16 current_time;

	
	//��һ��ʱ���
	timeStar1 = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd1 = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	set_time1 = timeEnd1 - timeStar1;
	
	//�ڶ���֮���
	timeStar2 = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd2 = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	set_time2 = timeEnd2 - timeStar2;
	
	//������֮���
	timeStar3 = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd3 = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	set_time3 = timeEnd3 - timeStar3;
	
	//��ǰʱ��(��λ�Ƿ���)
	current_time = calendar.hour*60+calendar.min;
	
	if(current_time<=timeStar1)		//��ǰʱ��û�����趨��ʱ���ڣ�˵����û�д����תʱ��
	{
		over_time = 0;
	}
	else if(current_time<=timeEnd1)		//��ǰʱ���ڵ�һ��ʱ����
	{
		over_time = current_time-timeStar1;		//�������תʱ��ʱ��
	}
	else if(current_time<=timeStar2)				//��ǰʱ���Ѿ����˵�һ��ʱ�䣬��û���ڶ���ʱ��
	{
		over_time = set_time1;
	}
	else if(current_time<=timeEnd2)			//�ڵڶ���ʱ����
	{
		over_time = current_time-timeStar2+set_time1;		//�������תʱ��ʱ��
	}
	else if(current_time<=timeStar3)			//�Ѿ�����ǰ������ʱ�䣬����û��������ʱ��
	{
		over_time = set_time1+set_time2;
	}
	else if(current_time<=timeEnd3)		//�ڵ�����ʱ����
	{
		over_time = current_time-timeStar3+set_time1+set_time2;
	}
	return over_time;
}

void MotorRest(void)
{

	uint16_t angle;					//�����ת�ܽǶ�
	uint32_t motor_run_time =0;		//�����ת��ʱ��
	uint16_t motor_step_sum = 0;	//���Ӧ����ת�Ĳ���
	uint16_t interval_sec = 0;		//���ִ��һ�ε����ת����
	uint16_t over_time;				//���Ӧ����ת������ʱ��(����)
	uint16_t current_step_1;		//��ǰӦ�õ���Ĳ���
	
	MotorGoBack();//�������ԭ��
	over_time = MotorGotoCurrentPoint(LED_MOTOR_STEP_MAX,calendar,motorInfo);
	motor_run_time = GetMotorRunTimer();	//��ȡ���ʱ��
	angle = motorInfo.angle_h*100+motorInfo.angle_l;	
	motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//��������Ҫ��ת�Ĳ���(�����һ��5��)
	interval_sec = motor_run_time/(motor_step_sum/5);		//ִ��һ��5��
	current_step_1 = motor_step_sum*over_time*60/motor_run_time;
	MotorGoTo(current_step_1);			//���ٵ��ﵱǰʱ��Ӧ�õ��ĽǶ�
}


uint16_t GetMotorCurrentStep(void)
{
	return motor_current_step;
}

uint16_t SetMotoCurrentStep(uint16_t step)
{
	motor_set_step = step;
}

void MotorStepRefresh(void)
{
    uint8_t step;	//���ú͵�ǰ��������
	
	//step = fabs(motor_current_step-motor_set_step);  //�����ֵ
	
	//if(step > 5)   //���ú͵�ǰ�������5����ִ�е����ת
	//{
		if(motor_current_step > motor_set_step)			//��ǰ�����������õĲ�������ôҪ��ת
		{
			MotorStart(0,2000);			//��ת
		}
		else if(motor_current_step < motor_set_step)
		{
			MotorStart(1,2000);			//��ת
		}
	//}
}


//��ǰʱ����������
u32 GetMotorMissSec(_calendar_obj time)
{
	uint32_t over_time = 0;		//�Ѿ���ȥ��ʱ�䣨��λ�Ƿ��ӣ�
	uint16_t set_time1,set_time2,set_time3;		//����趨��ʱ���
	u16 timeStar1, timeEnd1,timeStar2, timeEnd2,timeStar3, timeEnd3;		//������õ�ʱ�俪ʼ��ʱ�����
	u16 current_time;

	
	//��һ��ʱ���
	timeStar1 = motorInfo.s_hour1 * 60 + motorInfo.s_minute1;
	timeEnd1 = motorInfo.e_hour1 * 60 + motorInfo.e_minute1;
	set_time1 = timeEnd1 - timeStar1;
	
	//�ڶ���֮���
	timeStar2 = motorInfo.s_hour2 * 60 + motorInfo.s_minute2;
	timeEnd2 = motorInfo.e_hour2 * 60 + motorInfo.e_minute2;
	set_time2 = timeEnd2 - timeStar2;
	
	//������֮���
	timeStar3 = motorInfo.s_hour3 * 60 + motorInfo.s_minute3;
	timeEnd3 = motorInfo.e_hour3 * 60 + motorInfo.e_minute3;
	set_time3 = timeEnd3 - timeStar3;
	
	//��ǰʱ��(��λ�Ƿ���)
	current_time = calendar.hour*60+calendar.min;
	
	if(current_time<timeStar1)		//��ǰʱ��û�����趨��ʱ���ڣ�˵����û�д����תʱ��
	{
		over_time = 0;
	}
	else if(current_time<=timeEnd1)		//��ǰʱ���ڵ�һ��ʱ����
	{
		over_time = current_time-timeStar1;		//�������תʱ��ʱ��
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//������뵥λ,���ϵ�ǰ����ʱ��
	}
	else if(current_time<timeStar2)				//��ǰʱ���Ѿ����˵�һ��ʱ�䣬��û���ڶ���ʱ��
	{
		over_time = set_time1;
		over_time =  over_time*60;		//������뵥λ
	}
	else if(current_time<=timeEnd2)			//�ڵڶ���ʱ����
	{
		over_time = current_time-timeStar2+set_time1;		//�������תʱ��ʱ��
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//������뵥λ,���ϵ�ǰ����ʱ��
	}
	else if(current_time<timeStar3)			//�Ѿ�����ǰ������ʱ�䣬����û��������ʱ��
	{
		over_time = set_time1+set_time2;
		over_time =  over_time*60;		//������뵥λ
	}
	else if(current_time<=timeEnd3)		//�ڵ�����ʱ����
	{
		over_time = current_time-timeStar3+set_time1+set_time2;
		if(over_time>0) over_time=over_time-1;
		over_time =  over_time*60+calendar.sec;		//������뵥λ,���ϵ�ǰ����ʱ��
	}
	return over_time;
}

//��ǰʱ����Ӧ���߹��Ĳ���

u16 GetMotorMissStep(void)
{
	u16 RunSetp = 0;  //������е��ܲ���
	u16 RunSec = 0;		//��ʱ��
	u16 set_step = 0;	//��ǰʱ��Ӧ�����õĲ���
	u16 miss_sec = 0;
	
	RunSetp = GetMotorRunSetp();
	RunSec = GetMotorRunTimer();
	miss_sec = GetMotorMissSec(calendar);
	
	set_step =  (miss_sec* RunSetp)/RunSec;
	
	return set_step;
}
