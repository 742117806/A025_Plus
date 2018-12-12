/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 主函数，操作系统任务
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
OS_TID t_taskMotor;			//电机运行任务

void Init_Eeprom(void);

//static U64 stk_large[256/8]; //给uart分配大堆栈

static uint64_t taskUartProcStk[4096 / 8]; /* 任务栈 */

//改变PWM任务
__task void taskPwmChage(void)
{
	for (;;)
	{
		UpdatePWM();

		if (sysInfo.pwmSpeed) //速度不为0才延时
		{
			os_dly_wait(sysInfo.pwmSpeed);
		}
		
		os_tsk_pass();
	}
}

//时钟任务
__task void taskClock(void)
{
	u8 sec;
	u8 findRes;
	u8 findmoon;		//月相
	u8 oldRecSta;
	u8 CheckCnt;
	u8 sec10;
//	KeyValue_ENUM key = KEY_OFF;

	for (;;)
	{ 
		//按键处理
//		key=TaskKeyScan();

//		if (key != KEY_OFF)
//		{
//			ChangeMenu(key);
//		}
//				
		//每秒执行的任务
		if (sec != calendar.sec)
		{
			sec = calendar.sec;
					
			//***********************
			//定时检查串口接收是否正常
			//2秒钟内,接收标记没有变化，说明没有收到数据，就重置接收
			if (CheckCnt++ >1)
			{
				CheckCnt =0;

				if (oldRecSta == USART_RX_STA)//没收到数据，说明接收被中断了
				{
					USART_RX_STA = 0; //重置接收
				}
				oldRecSta = USART_RX_STA;
			}

			//每秒要执行的函数
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
			

			

			sysInfo.temperature = GetTemperature()/10; //取得温度
			if (sysInfo.temperature >= OVER_TMP)  //高于60度进入保护
			{
				sysInfo.OverProtect = 1;
				SetLedIndicator(LED_ITR_FLASH); //快速闪烁
			}else if (sysInfo.temperature <= OVER_TMP-10)
			{
				if (sysInfo.OverProtect)sysInfo.OverProtect = 0; //低于50度才清除保护
				SetLedIndicator(LED_ITR_ON); //亮起
			}

			if(!sysInfo.MunualFan) //如果不是手动设置转速
			{
				AdjFanByTmp();  //调整风扇转速
			}
			CheckUdataKey();
		}
		if (sysInfo.OverProtect)  //过温保护
		{
			//SetPwm(30, 30, 30, 20, 20, 20);
			SetPwm(1, 1, 1, 1, 1, 1);
		}
		else
		{
			//检查灯光特效及定时
			//优先权:特效 > 定时
			if (sysInfo.workmode == MODE_TIMER || sysInfo.workmode == MODE_CLUDE || sysInfo.workmode == MODE_FLASH
				||sysInfo.workmode == MODE_MOONLIGHT)
			{
				findRes = findCurrectEffect();  //优先处理特效
				//findmoon = findMoonLightEffect();//查找月相功能
				if ((!findRes) && (!findmoon))  //没有特效，转到定时模式
				{
					SetWorkMode(MODE_TIMER); //转成定时模式
					SetPwmByTimer(calendar.hour,calendar.min); 	//计算当前亮度 
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

				//灯光特效  ,
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
			STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //保存
		}
		LedIndicatorFlash(); //闪烁LED
		//BeepSong();//发声		

	 	os_dly_wait(2);
		os_tsk_pass();
	}
}

//显示任务
__task void taskDisplay(void)
{
	OS_RESULT evt_res;
	for (;;)
	{
		evt_res = os_evt_wait_or(0xFFFF,0xFFFE);
		if (evt_res != OS_R_TMO) //超时
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

//串口处理
__task void taskUartProc(void)
{
	uint16_t i;
	for (;;)
	{
		//处理串口数据
		if (USART_RECV_OK)
		{
			switch(USART_RX_TYPE)
			{
			case USART_TYYE_DATA: //数据传输
				SetClockFun(ShowUpdataProgress);

				if (sysInfo.updataType == UPDATA_FONT)
					SaveFontToFlash(); //升级字库
				else 
					SaveDataToFlash(); //升级APP

				USART_RX_STA = 0;
				break;
			case USART_TYPE_LED:  //LED指令
//				for(i=0;i<USART_RECV_COUNT;i++)
//				{
//					SEGGER_RTT_printf(0,"%02x ",USART_RX_BUF[i]);
//				}
//				SEGGER_RTT_printf(0,"\r\n ");
			 	LedCmdProc();
				USART_RX_STA = 0;
				break;
			case USART_TYPE_AT: //AT指令
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




//电机运行任务
__task void taskMotor(void)
{

	uint16_t motor_step_current = 0;//电机当前已旋转的步数
	uint16_t angle;					//电机旋转总角度
	uint8_t last_sec = 0;			//上一秒
	uint32_t motor_run_time =0;		//电机旋转总时间
	uint16_t motor_step_sum = 0;	//电机应该旋转的步数
	uint16_t interval_sec = 0;		//间隔执行一次电机旋转变量
	uint16_t interval_cnt = 0;		//间隔执行一次电机旋转计算
	uint16_t over_time;				//电机应该旋转经过的时间(分钟)
	uint16_t current_step_1;		//当前应该到达的步数
	uint16_t last_angle; 			//上次设置的角度
	

	#if 0	//模式1
	for (;;)
	{
		
		if (MotorIsEffective(calendar.hour, calendar.min) == 1)
		{


			if((calendar.sec != last_sec))
			{	
				last_sec = calendar.sec;
				
				angle = motorInfo.angle_h*100+motorInfo.angle_l;
	
				motor_step_current = GetMotorCurrentStep();
				motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//计算电机需要旋转的步数(电机动一下5步)
				
				motor_run_time = GetMotorRunTimer();	//获取电机时间
				interval_sec = motor_run_time/(motor_step_sum/5);		//执行一次5步
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
						MotorStart(1,5500);						//触发电机动一次（5步）
					}
				}
				if(angle!= last_angle)		//已经被设置过了
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
	#elif 0 		//模式二
	u8 speedCnt = 0;
	u8 dirFlag = 1;		//正转

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
				if((calendar.sec != last_sec))		//一秒执行一次
				{
					last_sec = calendar.sec;
					angle = motorInfo.angle_h*100+motorInfo.angle_l;
					motor_step_sum = (angle*LED_MOTOR_STEP_MAX/360);		//计算电机需要旋转的步数(电机动一下5步)
					motor_step_current = GetMotorCurrentStep();
					
					motor_run_time = GetMotorRunTimer();					//获取电机时间
					interval_sec = motor_run_time/(motor_step_sum/5);		//执行一次5步
					
					SEGGER_RTT_printf(0,"motor_step_current=%d\r\n",motor_step_current);

					
					#if 0
					speedCnt ++;
					if((speedCnt > ((10 - motorInfo.speed )*5)))		//*5为了减慢速度
					{
						speedCnt = 0;
						MotorStart(dirFlag,5500);						//触发电机动一次（5步）
						//SEGGER_RTT_printf(0,"step=%d\r\n",motor_step_current);
						if(motor_step_current > motor_step_sum )
						{
							//SEGGER_RTT_printf(0,"1111111\r\n");
							SetMotoCurrentStep(0);		//清零转动步数
							dirFlag ^= 1; //反转
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
						MotorStart(dirFlag,5500);						//触发电机动一次（5步）
						//SEGGER_RTT_printf(0,"step=%d\r\n",motor_step_current);
						if(motor_step_current > motor_step_sum )
						{
							//SEGGER_RTT_printf(0,"1111111\r\n");
							SetMotoCurrentStep(0);		//清零转动步数
							dirFlag ^= 1; //反转
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
//任务初始化
__task void taskInit(void)
{

	SetEffectFun(NullEffect);
	SetClockFun(Desktop);

	//t_taskUartProc = os_tsk_create(taskUartProc,1);

	t_taskUartProc = os_tsk_create_user(taskUartProc, 1, &taskUartProcStk, sizeof(taskUartProcStk)); //给这个任务单独分配堆栈
	t_taskClock = os_tsk_create(taskClock, 2);
	t_taskDisplay = os_tsk_create(taskDisplay,4);
	t_taskPwmChage = os_tsk_create(taskPwmChage,6);
	t_taskMotor = os_tsk_create(taskMotor,7);
	ESP8266_Init();
	//t_taskClock = os_tsk_create_user(taskClock,2,&stk_large,sizeof(stk_large)); //给这个任务单独分配堆栈
	//t_taskUartProc = os_tsk_create_user(taskUartProc,1,&stk_large,sizeof(stk_large)); //给这个任务单独分配堆栈
	
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
	//SCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET; //修改中断向量
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	KEY_Updata_Init(); //  升级按键
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
		MotorCCW();   //顺时针转动
	}
 }
#else

 int main(void)
 {	

	SystemInit();
	//SCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET; //修改中断向量
	delay_init();	    	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	//启动WIFI模块
	PWM_Init();
	FAN_init();
	
 	LED_Init();		//指示灯
	HLK_M30_IoInit();
	uart_init(115200);
	OLED_Init();
	ShowString(0, 0, "     Welcome      ");
	OLED_Refresh_Gram();
	//EC11_KeyInit();
	SetUartRxType(USART_TYPE_LED); //默认为灯控指令
	RTC_Init();  //时钟
	
	//Beep_Init();//蜂鸣器
	
	Init_Eeprom();
	Adc_Init();  //按键和温度
	ReadGUID();
	SPI_Flash_Init();
	MotorInit();
	//MotorBackOrigin();
	//initMessage(); //开机LOGO
	//OLED_DrawImg128X32(gImage_LOGO);
	KEY_Updata_Init(); //  升级按键
#if(LOGO_EN)
	OLED_MoveImg128X32(gImage_LOGO); //显示LOGO
#endif
	SetLedIndicator(LED_ITR_ONE);

	if(Get_ChipID()==0)	//测试芯片是否为指定芯片（调试用）
	{

		while(1)
		{
//		   	SEGGER_RTT_printf(0,"123456") ;
			delay_ms(1000);
		}
	}
	else
	{
		os_sys_init(taskInit); //启动RL_RTX OS
	}
	
	return 0;
}
#endif
 extern TIMER manualPwm;
 //初始化EEPROM数据
 void Init_Eeprom(void)
 {
	 u16 temp;

	 STM_FLASH_READ(FLASH_SYS_ADDR,&temp,sizeof(temp)); //读取
	 if (temp != 0x5A) //第一次运行,初始化数据
	 {
		 SystemLoadDefault(); //恢复默认
		 SavaTouchParam();  //保存按键校准数据

		 temp = 0x5A;
		 STM_FLASH_WRITE(FLASH_SYS_ADDR,&temp,sizeof(temp)); //写入标志
	 }

//	 STM_FLASH_READ(FLASH_INFO_ADDR,(u16*)&Sysflag,sizeof(Sysflag)); //系统信息
	 STM_FLASH_READ(FLASH_TIMER_ADDR,(u16*)TimerProg,sizeof(TimerProg));//PWM信息
	 
	 STM_FLASH_READ(FLASH_LANGUAGE_ADDR,(u16*)&temp,sizeof(temp)); //读语言

	 STM_FLASH_READ(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //读sysflag

	 STM_FLASH_READ(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));		//读取数据

	 if(temp > 1)temp = 1;
	 //sysInfo.language = (u8)temp;
	 sysInfo.language = ENGELISH;

	 ReadTouchParam();  //读取触摸数据

	 sysInfo.pwmNumber = GetPwmNumber();
	 if (sysInfo.pwmNumber == 0xff)
	 {
		 SetPwmNumber(6); //默认是6路软件
	 }

	 sysInfo.updataType = UPDATA_APP; //默认是升级APP
 }

 //-----------------end of file--------------------
