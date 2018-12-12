/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: ������������ϵͳ����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "include.h"
#include <RTL.h>
#include "func.h"
#include "logo.h"
#include "my_string.h"
#include "ESP8266_07S.h"
#include "uln2003.h"
#include "SEGGER_RTT.h"
#include "string.h"

const u8 version[2]={V1,V2};

extern void ChangeMenu(KeyValue_ENUM key);

OS_TID t_taskClock;
OS_TID t_taskUartProc;
OS_TID t_taskDisplay;
OS_TID t_taskPwmChage;
OS_TID t_taskMotor;			//�����������

void Init_Eeprom(void);

//static U64 stk_large[256/8]; //��uart������ջ

static uint64_t taskUartProcStk[4096 / 8]; /* ����ջ */

//�ı�PWM����
__task void taskPwmChage(void)
{
	for (;;)
	{
		UpdatePWM();

		if (sysInfo.pwmSpeed) //�ٶȲ�Ϊ0����ʱ
		{
			os_dly_wait(sysInfo.pwmSpeed);
		}
		
		os_tsk_pass();
	}
}

//ʱ������
__task void taskClock(void)
{
	u8 sec;
	u8 findRes;
	u8 findmoon;		//����
	u8 oldRecSta;
	u8 CheckCnt;
	u8 sec10;
//	KeyValue_ENUM key = KEY_OFF;

	for (;;)
	{ 
		//��������
//		key=TaskKeyScan();

//		if (key != KEY_OFF)
//		{
//			ChangeMenu(key);
//		}
//				
		//ÿ��ִ�е�����
		if (sec != calendar.sec)
		{
			sec = calendar.sec;
					
			//***********************
			//��ʱ��鴮�ڽ����Ƿ�����
			//2������,���ձ��û�б仯��˵��û���յ����ݣ������ý���
			if (CheckCnt++ >1)
			{
				CheckCnt =0;

				if (oldRecSta == USART_RX_STA)//û�յ����ݣ�˵�����ձ��ж���
				{
					USART_RX_STA = 0; //���ý���
				}
				oldRecSta = USART_RX_STA;
			}

			//ÿ��Ҫִ�еĺ���
			if (ClockFunc != NullFucn)
			{
				ClockFunc();
				OS_UPDATA_OLED();
			}
			sec10++;
			if (sec10 > 10)
			{
				sec10 = 0;
				//printf("AT+CWJAP?\r\n");
				
			}
			

			

			sysInfo.temperature = GetTemperature()/10; //ȡ���¶�
			if (sysInfo.temperature >= OVER_TMP)  //����60�Ƚ��뱣��
			{
				sysInfo.OverProtect = 1;
				SetLedIndicator(LED_ITR_FLASH); //������˸
			}else if (sysInfo.temperature <= OVER_TMP-10)
			{
				if (sysInfo.OverProtect)sysInfo.OverProtect = 0; //����50�Ȳ��������
				SetLedIndicator(LED_ITR_ON); //����
			}

			if(!sysInfo.MunualFan) //��������ֶ�����ת��
			{
				AdjFanByTmp();  //��������ת��
			}
			CheckUdataKey();
		}
		if (sysInfo.OverProtect)  //���±���
		{
			//SetPwm(30, 30, 30, 20, 20, 20);
			SetPwm(1, 1, 1, 1, 1, 1);
		}
		else
		{
			//���ƹ���Ч����ʱ
			//����Ȩ:��Ч > ��ʱ
			if (sysInfo.workmode == MODE_TIMER || sysInfo.workmode == MODE_CLUDE || sysInfo.workmode == MODE_FLASH
				||sysInfo.workmode == MODE_MOONLIGHT)
			{
				findRes = findCurrectEffect();  //���ȴ�����Ч
				//findmoon = findMoonLightEffect();//�������๦��
				if ((!findRes) && (!findmoon))  //û����Ч��ת����ʱģʽ
				{
					SetWorkMode(MODE_TIMER); //ת�ɶ�ʱģʽ
					SetPwmByTimer(calendar.hour,calendar.min); 	//���㵱ǰ���� 
				}
				else
				{
					//	SetWorkMode(MODE_WEATHER);
				}
			}

			switch(sysInfo.workmode)
			{
			case MODE_TIMER:
				SetPwmSpeed(PWM_SPEED_TIMER);
				break;

			case MODE_MUNUAL:
				SetPwmSpeed(PWM_SPEED_DIMMER);
				break;

				//�ƹ���Ч  ,
			case MODE_FLASH: 
			case MODE_CLUDE:
			case MODE_CLUDE_VEIW:
			case MODE_FLASH_VEIW:
			case MODE_MOONLIGHT:
			case MODE_MOONLIGHT_VEIW:
				EffectFunc();		
				break;

			case MODE_POWER_OFF:
				break;

			case MODE_PREVIEW:
				PreviewTimer();
				break;
			}
		}

		//LED0 = ~LED0;
		if (sysFlag.Bits.LED_SAVE)
		{
			sysFlag.Bits.LED_SAVE = 0;
			STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //����
		}
		LedIndicatorFlash(); //��˸LED
		//BeepSong();//����		

	 	os_dly_wait(2);
		os_tsk_pass();
	}
}

//��ʾ����
__task void taskDisplay(void)
{
	OS_RESULT evt_res;
	for (;;)
	{
		evt_res = os_evt_wait_or(0xFFFF,0xFFFE);
		if (evt_res != OS_R_TMO) //��ʱ
		{
			evt_res = os_evt_get();
			switch(evt_res)
			{
			case T_MSG_UPDATA_OLED:
				OLED_Refresh_Gram();
				break;
			}		
		}
		os_tsk_pass();
	}
}

//���ڴ���
__task void taskUartProc(void)
{
	uint16_t i;
	for (;;)
	{
		//����������
		if (USART_RECV_OK)
		{
			switch(USART_RX_TYPE)
			{
			case USART_TYYE_DATA: //���ݴ���
				SetClockFun(ShowUpdataProgress);

				if (sysInfo.updataType == UPDATA_FONT)
					SaveFontToFlash(); //�����ֿ�
				else 
					SaveDataToFlash(); //����APP

				USART_RX_STA = 0;
				break;
			case USART_TYPE_LED:  //LEDָ��
//				for(i=0;i<USART_RECV_COUNT;i++)
//				{
//					SEGGER_RTT_printf(0,"%02x ",USART_RX_BUF[i]);
//				}
//				SEGGER_RTT_printf(0,"\r\n ");
			 	LedCmdProc();
				USART_RX_STA = 0;
				break;
			case USART_TYPE_AT: //ATָ��
				break;
			}
		}
		if(WiFiRespointstr.rcvOk)
		{

			
			WiFiRespointstr.rcvOk = 0;
			WiFiRespointstr.cnt = 0;
			if(strncmp("WIFI CONNECTED",(const char*)WiFiRespointstr.strBuf,14)==0)
			{
				//SEGGER_RTT_printf(0,"\r\nWIFI CONNECTED\r\n");
				wifiConnetFlag = 1;
			}
			else if(strncmp("WIFI DISCONNECT",(const char*)WiFiRespointstr.strBuf,15)==0)
			{
				
				//SEGGER_RTT_printf(0,"\r\nWIFI DISCONNECT\r\n");
				wifiConnetFlag = 0;
			}
			
		}
	 	os_tsk_pass();
	}
}




//�����������
__task void taskMotor(void)
{

	uint16_t motor_step_current = 0;//�����ǰ����ת�Ĳ���
	uint16_t angle;					//�����ת�ܽǶ�
	uint8_t last_sec = 0;			//��һ��
	uint32_t motor_run_time =0;		//�����ת��ʱ��
	uint16_t motor_step_sum = 0;	//���Ӧ����ת�Ĳ���
	uint16_t interval_sec = 0;		//���ִ��һ�ε����ת����
	uint16_t interval_cnt = 0;		//���ִ��һ�ε����ת����
	uint16_t over_time;				//���Ӧ����ת������ʱ��(����)
	uint16_t current_step_1;		//��ǰӦ�õ���Ĳ���
	uint16_t last_angle; 			//�ϴ����õĽǶ�
	

	#if 0	//ģʽ1
	for (;;)
	{
		
		if (MotorIsEffective(calendar.hour, calendar.min) == 1)
		{


			if((calendar.sec != last_sec))
			{	
				last_sec = calendar.sec;
				
				angle = motorInfo.angle_h*100+motorInfo.angle_l;
	
				motor_step_current = GetMotorCurrentStep();
				motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//��������Ҫ��ת�Ĳ���(�����һ��5��)
				
				motor_run_time = GetMotorRunTimer();	//��ȡ���ʱ��
				interval_sec = motor_run_time/(motor_step_sum/5);		//ִ��һ��5��
				over_time = MotorGotoCurrentPoint(LED_MOTOR_STEP_MAX,calendar,motorInfo);
				current_step_1 = (over_time*60*5)/interval_sec;
				
//				SEGGER_RTT_printf(0,"over_time = %d\r\n",over_time);
//				SEGGER_RTT_printf(0,"motor_run_time = %d\r\n",motor_run_time);
//				SEGGER_RTT_printf(0,"motor_step_sum = %d\r\n",motor_step_sum);
//				SEGGER_RTT_printf(0,"interval_sec = %d\r\n",interval_sec);
//				SEGGER_RTT_printf(0,"angle = %d\r\n",angle);
//				SEGGER_RTT_printf(0,"motor_step_current = %d\r\n",motor_step_current);
//				SEGGER_RTT_printf(0,"motor_step_current_1 = %d\r\n",current_step_1);

				
				interval_cnt ++;
				if(interval_cnt >=interval_sec)
				{
					interval_cnt = 0;
					if(motor_step_current < motor_step_sum)
					{
						//SEGGER_RTT_printf(0,"MotorStart RUN = %d\r\n");
						MotorStart(1,5500);						//���������һ�Σ�5����
					}
				}
				if(angle!= last_angle)		//�Ѿ������ù���
				{
					//SEGGER_RTT_printf(0,"angle!= last_angle\r\n angle = %d",angle);
					MotorRest();
					last_angle = angle;
				}
			}
						
			
		}
		MotorAutoGoBack(calendar);
		os_dly_wait(4);
		os_tsk_pass();
		
	}
	#elif 0 		//ģʽ��
	u8 speedCnt = 0;
	u8 dirFlag = 1;		//��ת

	MotorStop();
	//MotorGoBack(LED_MOTOR_STEP_MAX);
	SetMotoCurrentStep(0);
	

	//MotorRun(1500,0);
	speedCnt = 0;// speed:0-10
	
	for (;;)
	{
		if(motorInfo.speed > 0)
		{
			if (MotorIsEffective(calendar.hour, calendar.min) == 1)
			{
				if((calendar.sec != last_sec))		//һ��ִ��һ��
				{
					last_sec = calendar.sec;
					angle = motorInfo.angle_h*100+motorInfo.angle_l;
					motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//��������Ҫ��ת�Ĳ���(�����һ��5��)
					motor_step_current = GetMotorCurrentStep();
					
					motor_run_time = GetMotorRunTimer();					//��ȡ���ʱ��
					interval_sec = motor_run_time/(motor_step_sum/5);		//ִ��һ��5��
					
					SEGGER_RTT_printf(0,"motor_step_current=%d\r\n",motor_step_current);

					
					#if 0
					speedCnt ++;
					if((speedCnt > ((10 - motorInfo.speed )*5)))		//*5Ϊ�˼����ٶ�
					{
						speedCnt = 0;
						MotorStart(dirFlag,5500);						//���������һ�Σ�5����
						//SEGGER_RTT_printf(0,"step=%d\r\n",motor_step_current);
						if(motor_step_current > motor_step_sum )
						{
							//SEGGER_RTT_printf(0,"1111111\r\n");
							SetMotoCurrentStep(0);		//����ת������
							dirFlag ^= 1; //��ת
						}
					}	
					#else 	
					interval_cnt ++;
	
					if(interval_cnt >= interval_sec)
					{
						if(interval_sec == 0)
						{
							motorInfo.speed = 19;
						}
						else
						{
							motorInfo.speed = 4;
						}
						interval_cnt = 0;
						MotorStart(dirFlag,5500);						//���������һ�Σ�5����
						//SEGGER_RTT_printf(0,"step=%d\r\n",motor_step_current);
						if(motor_step_current > motor_step_sum )
						{
							//SEGGER_RTT_printf(0,"1111111\r\n");
							SetMotoCurrentStep(0);		//����ת������
							dirFlag ^= 1; //��ת
						}
					}

					#endif
				}
			}
		}
		os_dly_wait(4);
		os_tsk_pass();
	}
	
	#else 
    u16 set_step = 0;
	
	MotorGoBack();
	#if 1
	for(;;)
	{
//		SEGGER_RTT_printf(0,"motor_current_step = %d\r\n",motor_current_step);        
//		SEGGER_RTT_printf(0,"MotorRunSetp = %d\r\n",GetMotorRunSetp());		
//		SEGGER_RTT_printf(0,"MotorRunTimer = %d\r\n",GetMotorRunTimer());
//		SEGGER_RTT_printf(0,"MissMinute = %d\r\n",GetMotorMissSec(calendar));
		set_step = GetMotorMissStep();
		if(motorInfo.enable == 1)
		{
			if(set_step>0)
			{
				SetMotoCurrentStep(set_step);
				//SEGGER_RTT_printf(0,"set_step = %d\r\n",set_step);
			}
			MotorStepRefresh();
		}
		else
		{
		   MotorStop();
		}
		os_dly_wait(100);
		
	}
	#else
	set_step = LED_MOTOR_STEP_MAX;
	SetMotoCurrentStep(set_step);
	for(;;)
	{
//		SEGGER_RTT_printf(0,"motor_current_step = %d\r\n",motor_current_step);  
		MotorStepRefresh();
		os_dly_wait(100);
	}
	#endif
	
	
	#endif
}
//�����ʼ��
__task void taskInit(void)
{

	SetEffectFun(NullEffect);
	SetClockFun(Desktop);

	//t_taskUartProc = os_tsk_create(taskUartProc,1);

	t_taskUartProc = os_tsk_create_user(taskUartProc, 1, &taskUartProcStk, sizeof(taskUartProcStk)); //��������񵥶������ջ
	t_taskClock = os_tsk_create(taskClock, 2);
	t_taskDisplay = os_tsk_create(taskDisplay,4);
	t_taskPwmChage = os_tsk_create(taskPwmChage,6);
	t_taskMotor = os_tsk_create(taskMotor,7);
	ESP8266_Init();
	//t_taskClock = os_tsk_create_user(taskClock,2,&stk_large,sizeof(stk_large)); //��������񵥶������ջ
	//t_taskUartProc = os_tsk_create_user(taskUartProc,1,&stk_large,sizeof(stk_large)); //��������񵥶������ջ
	
	os_tsk_delete_self();
}
#if 0

int main(void)
{
	u16 i = 0;
	u16 i2 = 0;
	u32 time_cnt = 0;

	u8 flag = 0;
	SystemInit();
	//SCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET; //�޸��ж�����
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	KEY_Updata_Init(); //  ��������
	uart_init(115200);
	MotorInit();

 
	MotorInit();
	while(1)
	{
		time_cnt++;
		if(time_cnt>99)
		{
			time_cnt = 0;
			LED0 ^= 1;
		}
		MotorCCW();   //˳ʱ��ת��
	}
 }
#else

 int main(void)
 {	

	SystemInit();
	//SCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET; //�޸��ж�����
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�

	//����WIFIģ��
	PWM_Init();
	FAN_init();
	
 	LED_Init();		//ָʾ��
	HLK_M30_IoInit();
	uart_init(115200);
	OLED_Init();
	ShowString(0, 0, "     Welcome      ");
	OLED_Refresh_Gram();
	//EC11_KeyInit();
	SetUartRxType(USART_TYPE_LED); //Ĭ��Ϊ�ƿ�ָ��
	RTC_Init();  //ʱ��
	
	//Beep_Init();//������
	
	Init_Eeprom();
	Adc_Init();  //�������¶�
	ReadGUID();
	SPI_Flash_Init();
	MotorInit();
	//MotorBackOrigin();
	//initMessage(); //����LOGO
	//OLED_DrawImg128X32(gImage_LOGO);
	KEY_Updata_Init(); //  ��������
#if(LOGO_EN)
	OLED_MoveImg128X32(gImage_LOGO); //��ʾLOGO
#endif
	SetLedIndicator(LED_ITR_ONE);

	if(Get_ChipID()==0)	//����оƬ�Ƿ�Ϊָ��оƬ�������ã�
	{

		while(1)
		{
//		   	SEGGER_RTT_printf(0,"123456") ;
			delay_ms(1000);
		}
	}
	else
	{
		os_sys_init(taskInit); //����RL_RTX OS
	}
	
	return 0;
}
#endif
 extern TIMER manualPwm;
 //��ʼ��EEPROM����
 void Init_Eeprom(void)
 {
	 u16 temp;

	 STM_FLASH_READ(FLASH_SYS_ADDR,&temp,sizeof(temp)); //��ȡ
	 if (temp != 0x5A) //��һ������,��ʼ������
	 {
		 SystemLoadDefault(); //�ָ�Ĭ��
		 SavaTouchParam();  //���水��У׼����

		 temp = 0x5A;
		 STM_FLASH_WRITE(FLASH_SYS_ADDR,&temp,sizeof(temp)); //д���־
	 }

//	 STM_FLASH_READ(FLASH_INFO_ADDR,(u16*)&Sysflag,sizeof(Sysflag)); //ϵͳ��Ϣ
	 STM_FLASH_READ(FLASH_TIMER_ADDR,(u16*)TimerProg,sizeof(TimerProg));//PWM��Ϣ
	 
	 STM_FLASH_READ(FLASH_LANGUAGE_ADDR,(u16*)&temp,sizeof(temp)); //������

	 STM_FLASH_READ(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //��sysflag

	 STM_FLASH_READ(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));		//��ȡ����

	 if(temp > 1)temp = 1;
	 //sysInfo.language = (u8)temp;
	 sysInfo.language = ENGELISH;

	 ReadTouchParam();  //��ȡ��������

	 sysInfo.pwmNumber = GetPwmNumber();
	 if (sysInfo.pwmNumber == 0xff)
	 {
		 SetPwmNumber(6); //Ĭ����6·���
	 }

	 sysInfo.updataType = UPDATA_APP; //Ĭ��������APP
 }

 //-----------------end of file--------------------
