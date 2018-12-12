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

#include "CmdProc.h"
#include "func.h"
#include "timer.h"
#include "FAN.h"
#include "usart.h"
#include "rtc.h"
#include "system.h"
#include "stmflash.h"
#include "temperature.h"
#include "OtherProc.h"
#include "pwmData.h"
#include "weather.h"
#include "TimerFunc.h"
#include "menu.h"
#include "ESP8266_07S.h"
#include <string.h>
#include "rtl.h"


u8 respon; //��Ӧ����

//����ڴ�
void ClearRam(u8 *addr,u16 len)
{
	u8 *p;
	p= addr;
	while(len)
	{
		*p++ = 0x00;
		len--;
	}
}

//����ʱ��
void SetTimeProc(void)
{
	RTC_Set(
		2000+pTime->year, //�����2000��Ϊ��׼���ϼ�
		pTime->month,
		pTime->day,
		pTime->hour,
		pTime->minute,
		pTime->second
		); //����ʱ��

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//��ȡʱ��
void GetTimeProc(void)
{
	STRUCT_TIME time;

	ClearRam((u8*)&time,sizeof(time));

	time.start = FRAME_START;
	time.type = FRAME_TYPE_TIME;
	time.mode = FRAME_MODE_GET;
	time.year = calendar.w_year-2000;
	time.month = calendar.w_month;
	time.day = calendar.w_date;
	time.hour = calendar.hour;
	time.minute = calendar.min;
	time.second = calendar.sec;
	time.end = FRAME_END;

	//uart_sendbuf((u8*)&time,sizeof(time));
	ESP8266_SendDatas(current_linkID, (u8*)&time, sizeof(time));
}

//��ȡϵͳ��Ϣ
void GetSystemInfoProc()
{
	STRUCT_SYSTEM sys;
	u16 custom_cdoe; //
	unsigned int mac;
	extern u8 CPU_GUID[12]; //96λоƬΨһ��ʶ
	ClearRam((u8*)&sys,sizeof(sys));

	custom_cdoe =GetCustomCode(); //��ȡ�ͻ�����

	sys.start = FRAME_START;
	sys.type = FRAME_TYPE_SYSTEM;
	sys.mode = FRAME_MODE_GET;
	sys.command = FRAME_CMD_EXIST;

	sys.para[SYSTEM_TEMP] = sysInfo.temperature;//GetTemperature()/10; //�¶�;
	sys.para[SYSTEM_VERSION1] = version[0]; //����汾1
	sys.para[SYSTEM_VERSION2] = version[1]; //����汾2
	sys.para[SYSTEM_EXIST]  = 0x5A;	//����ȷ��,��������0x5A
	sys.para[SYSTEM_FANSPEED] = GetFanSpeed(); //����ת��
	sys.para[SYSTEM_CUSTOM_CODE1] = custom_cdoe>>8;  //�ͻ�����
	sys.para[SYSTEM_CUSTOM_CODE2] = (u8)custom_cdoe;
	sys.para[SYSTEM_MODE] = sysInfo.workmode; //��ǰģʽ
	sys.para[SYSTEM_PWM_NUM] = GetPwmNumber(); //����·��
	sys.para[SYSTEM_TIMER_INDEX] = sysInfo.indexTimer+1; //��ǰ��ʱ�� ��1~ 12

	sys.para[SYSTEM_LED_STATE] = sysFlag.Bits.LED_sta; //ָʾ��״̬

	//ȡWFIF ģ��MAC�ĺ���2λ��ʮ������
	sscanf((const char*)CPU_GUID+4, "%x", &mac);
	
	sys.para[SYSTEM_GUID0] = 0; 
	sys.para[SYSTEM_GUID1] = ((mac>>8)&0x00ff);
	sys.para[SYSTEM_GUID2] = (mac&0x00ff);

	sys.end = FRAME_END;
	
	
	ESP8266_SendDatas(current_linkID, (u8*)&sys, sizeof(sys));

}


//ת�������̼�ģʽ
void SetUpdataModeProc(void)
{
	respon = RET_OK;
	//uart_sendbuf(&respon,1);	
	ESP8266_SendDatas(current_linkID, &respon, 1);
	SetUartRxType(USART_TYYE_DATA); 
	USART_RX_STA = 0;
	sysInfo.updataType = pSystem->para[SYSTEM_MODE]; //ָ����������:
}


//����PWMֵ
void SetDimmerProc(void)
{
	SetPwm(
		pDimmer->pwm1,
		pDimmer->pwm2,
		pDimmer->pwm3,
		pDimmer->pwm4,
		pDimmer->pwm5,
		pDimmer->pwm6
		);

	SetWorkMode(MODE_MUNUAL); //ת���ֶ�ģʽ

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//����PWMֵ
void GetDimmerProc(void)
{
	STRUCT_DIMMER dimmer;
	
	ClearRam((u8*)&dimmer,sizeof(dimmer));

	dimmer.start = FRAME_START;
	dimmer.type = FRAME_TYPE_DIMMER;
	dimmer.mode = FRAME_MODE_SET;
	dimmer.pwm1 = GetPwm(1);
	dimmer.pwm2 = GetPwm(2);
	dimmer.pwm3 = GetPwm(3);
	dimmer.pwm4 = GetPwm(4);
	dimmer.pwm5 = GetPwm(5);
	dimmer.pwm6 = GetPwm(6);
	dimmer.end = FRAME_END;

	//uart_sendbuf((u8*)&dimmer,sizeof(dimmer));
	
	ESP8266_SendDatas(current_linkID, (u8*)&dimmer, sizeof(dimmer));
}


//����FAN
void SetFanSpeedProc(void)
{
	if (pSystem->para[SYSTEM_FANSPEED] > 100) //����ֵ����100�ͻص��Զ��¿�ģʽ
	{
		sysInfo.MunualFan = 0;
	}
	else
	{
		SetFanSpeed(pSystem->para[SYSTEM_FANSPEED]);
		sysInfo.MunualFan = 1; //�����ֶ�ת��
	}	
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}


//���ÿͻ���ʶ����
void SetCustomCodeProc(void)
{
	u16 custom_cdoe;

	custom_cdoe = pSystem->para[SYSTEM_CUSTOM_CODE1];
	custom_cdoe = custom_cdoe<<8 | pSystem->para[SYSTEM_CUSTOM_CODE2];
	SetCustomCode(custom_cdoe);

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//�ָ���Ĭ������
void SetDefaultProc(void)
{
	SystemLoadDefault();
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//���ù���ģʽ
void SetWorkModeProc(void)
{
	SetWorkMode(pSystem->para[SYSTEM_MODE]); 

	if (sysInfo.workmode == MODE_POWER_OFF)//�ص�ģʽ
	{
		SetPwmA(0,0,0,0,0,0);
	}

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//���õ���·��
void SetPwmNuberProc(void)
{
	u8 number;
	number = pSystem->para[SYSTEM_PWM_NUM];
	if (number == 3 || number == 6)//ֻ����3·��6·
	{
		SetPwmNumber(number);
		sysInfo.pwmNumber = number;
		respon = RET_OK;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
	}
	else
	{
		respon = RET_NG;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(0, &respon, 1);
	}
}

//���ù���ָʾ��״̬
void SetLedStateProc(void)
{
	if (pSystem->para[SYSTEM_LED_STATE])sysFlag.Bits.LED_sta = 1;
	else sysFlag.Bits.LED_sta = 0;

	 STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //����

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//����WIFI STA����
/*
para[0]:Ҫ���õ�WIFI����,1=WIFI����;2=WIFI����;3=���ܷ�ʽ
para[1]~para[14]:���õ����� 
*/
void SetWifiStaConnProc(void)
{
	if (pSystem->para[0]==1)
	{
		respon = RET_OK;
		//uart_sendbuf(&respon,1);
		ESP8266_SendDatas(current_linkID, &respon, 1);
		ConfigWifiStaConn();
	}
	
}

//ȡ��ʱ����
void GetTimerProc()
{
	STRUCT_TIMER tmr;
	TIMER tm_step;
	u8 addr;
	u8 len;
		
	ClearRam((u8*)&tmr,sizeof(tmr));
	//STM32��FLASH��16λ���룬���Զ�ȡ���ֽ���������2�ֽڵı���
	//���գ���Ϊʵ�������ڴ���9�ֽڣ���ʵ�ʶ�ȡ��10���ֽڣ����ܰ�������������
	len = sizeof(TIMER);  //sizeof�����9��Ҫת��10��
	if (len%2)len=len+1;

	//��λ���Ķ�ʱʱ�������1,2,3,4,5...����Ƭ����0,1,2,��ʼ��Ҫת��һ��
	//���������index��0,Ĭ�϶�ȡ��ʱ1
	if (pTimer->index >0 && pTimer->index <=PRGM_STEP)//����Ƿ�Ϸ�
	{
		addr =sizeof(TIMER)* (pTimer->index - 1);

		STM_FLASH_READ(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//��ȡPWM��Ϣ

		tmr.start = FRAME_START;
		tmr.type = FRAME_TYPE_TIMER;
		tmr.mode = FRAME_MODE_GET;
		tmr.index = pTimer->index;
		tmr.enable = tm_step.enable;
		tmr.hour = tm_step.hour;
		tmr.minute = tm_step.minute;
		tmr.pwm1 = tm_step.pwm1;
		tmr.pwm2 = tm_step.pwm2;
		tmr.pwm3 = tm_step.pwm3;
		tmr.pwm4 = tm_step.pwm4;
		tmr.pwm5 = tm_step.pwm5;
		tmr.pwm6 = tm_step.pwm6;
		tmr.count = GetMaxTimerStep(); //��ȡ��ʱ������
		tmr.end  =	FRAME_END;

		//uart_sendbuf((u8*)&tmr,sizeof(tmr));
		ESP8266_SendDatas(current_linkID, (u8*)&tmr, sizeof(tmr));
	}
	else
	{
		//���ض�ʱ������
		tmr.start = FRAME_START;
		tmr.type = FRAME_TYPE_TIMER;
		tmr.mode = FRAME_MODE_GET;
		tmr.index = 0;
		tmr.enable = 0;
		tmr.hour = 0;
		tmr.minute = 0;
		tmr.pwm1 = 0;
		tmr.pwm2 = 0;
		tmr.pwm3 = 0;
		tmr.pwm4 = 0;
		tmr.pwm5 = 0;
		tmr.pwm6 = 0;
		tmr.count = GetMaxTimerStep(); //��ȡ��ʱ������
		tmr.end  =	FRAME_END;
		//uart_sendbuf((u8*)&tmr,sizeof(tmr));
		ESP8266_SendDatas(current_linkID, (u8*)&tmr, sizeof(tmr));
	}

}

//���ö�ʱ
void SetTimerProc(void)
{
	TIMER tm_step;
	u16 addr;
	u8 len;
	u8 i;
	u8 index; //����
	u8 count; //��ʱ����

	//STM32��FLASH��16λ���룬���Զ�ȡ���ֽ���������2�ֽڵı���
	//���գ���Ϊʵ�������ڴ���9�ֽڣ���ʵ�ʶ�ȡ��10���ֽڣ����ܰ�������������
	len = sizeof(TIMER);  //sizeof�����9��Ҫת��10��
	if (len%2)len=len+1;

	index = pTimer->index;
	count = pTimer->count;

	//��鶨ʱʱ���Ƿ�Ϸ� 
	if (index >0 && index <=PRGM_STEP)
	{
		tm_step.enable = pTimer->enable;
		tm_step.hour = pTimer->hour;
		tm_step.minute = pTimer->minute;
		tm_step.pwm1 = pTimer->pwm1;
		tm_step.pwm2 = pTimer->pwm2;
		tm_step.pwm3 = pTimer->pwm3;
		tm_step.pwm4 = pTimer->pwm4;
		tm_step.pwm5 = pTimer->pwm5;
		tm_step.pwm6 = pTimer->pwm6;

		addr =sizeof(TIMER)*(index-1);
		STM_FLASH_WRITE(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//д��PWM��Ϣ

		if (index == count) //��������һ����ʱ��,��ʣ�µĶ�ʱȫ���ر�
		{
			for(i=count;i<PRGM_STEP;i++)
			{
				tm_step.enable = 0;
				addr =sizeof(TIMER)* i;
				STM_FLASH_WRITE(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//д��PWM��Ϣ
			}
		}

		SetWorkMode(MODE_TIMER);
		respon = RET_OK;
	}
	else if(pTimer->index == 0) //index=0 ���Ƕ�ʱԤ��
	{
		if (pTimer->enable)  //��ʾ��ʱԤ��
		{
			SetClockFun(NullFucn);
			SetPreveiwTime(pTimer->hour,pTimer->minute);
			PreviewTimer(); //	
			ResetMenuToDestop(); //�˵����ص�����
		}
		else   //�رն�ʱԤ��
		{
			SetWorkMode(MODE_TIMER);
			SetClockFun(Desktop);
			ResetPreviewTimer();
		}
		respon = RET_OK;
	}
	else
	{
		respon = RET_NG;
	}
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//ȡ�ƹ���Ч
void GetEffectProc(void)
{
	STRUCT_WEATHER effect;
	WEATHER eff_step;
	u8 addr;
	u8 len;

	ClearRam((u8*)&effect,sizeof(effect));
	//STM32��FLASH��16λ���룬���Զ�ȡ���ֽ���������2�ֽڵı���
	//���գ���Ϊʵ�������ڴ���9�ֽڣ���ʵ�ʶ�ȡ��10���ֽڣ����ܰ�������������
	len = sizeof(WEATHER);  //sizeof�����9��Ҫת��10��
	if (len%2)len=len+1;

	//��λ���Ķ�ʱʱ�������1,2,3,4,5...����Ƭ����0,1,2,��ʼ��Ҫת��һ��
	//���������index��0,Ĭ�϶�ȡ��ʱ1
	if (pWeather->index >0 && pWeather->index <=WEATHER_SETP)//����Ƿ�Ϸ�
	{
		addr =sizeof(WEATHER)* (pWeather->index - 1);

		STM_FLASH_READ(FLASH_EFFECT_ADDR+addr,(u16*)&eff_step,len);//��ȡPWM��Ϣ

		effect.start = FRAME_START;
		effect.type = FRAME_TYPE_WEATHER;
		effect.mode = FRAME_MODE_GET;
		effect.index = pWeather->index;
		effect.enable = eff_step.enable;
		effect.effect = eff_step.effect;
		effect.level = eff_step.level;
		effect.s_hour = eff_step.s_hour;
		effect.s_minute = eff_step.s_minute;
		effect.e_hour = eff_step.e_hour;
		effect.e_minute = eff_step.e_minute;
			
		effect.count = GetMaxEffectStep(); //��ȡ��Ч����
		effect.end  =	FRAME_END;
	}
	else
	{
		//���ض�ʱ������
		effect.start = FRAME_START;
		effect.type = FRAME_TYPE_WEATHER;
		effect.mode = FRAME_MODE_GET;
		effect.index = 0;
		effect.enable = 0;
		effect.effect = 0;
		effect.level = 0;
		effect.s_hour = 0;
		effect.s_minute = 0;
		effect.e_hour = 0;
		effect.e_minute = 0;

		effect.count = GetMaxEffectStep(); //��ȡ��Ч����
		effect.end  =	FRAME_END;
	}
	//uart_sendbuf((u8*)&effect,sizeof(effect));
	ESP8266_SendDatas(current_linkID, (u8*)&effect, sizeof(effect));
}

//���õƹ���Ч
void SetEffectProc(void)
{
	u8 len;
	u16 addr;
	u8 index;
	u8 count;
	u8 i;
	WEATHER effect_step;

	index = pWeather->index;
	count = pWeather->count;

	//pWeather->level = 100 - pWeather->level;  //Ҫת��һ��

	if(index >0 && index <=WEATHER_SETP)  //������Ч
	{
		len = sizeof(WEATHER);  //sizeof�����9��Ҫת��10��
		if (len%2)len=len+1;

		effect_step.enable = pWeather->enable;
		effect_step.level = pWeather->level;
		effect_step.effect = pWeather->effect;
		effect_step.s_hour = pWeather->s_hour;
		effect_step.s_minute = pWeather->s_minute;
		effect_step.e_hour = pWeather->e_hour;
		effect_step.e_minute = pWeather->e_minute;
		
		addr =sizeof(WEATHER)*(index-1);
		STM_FLASH_WRITE(FLASH_EFFECT_ADDR+addr,(u16*)&effect_step,len);//д������

		if (index == count) //��������һ����ʱ��,��ʣ�µĶ�ʱȫ���ر�
		{
			for(i=count;i<WEATHER_SETP;i++)
			{
				effect_step.enable = 0;
				addr =sizeof(WEATHER)* i;
				STM_FLASH_WRITE(FLASH_EFFECT_ADDR+addr,(u16*)&effect_step,len);//д��PWM��Ϣ
			}
		}

		SetWorkMode(MODE_TIMER);
		respon = RET_OK;
	}
	else if (pWeather->index == 0) //Ԥ��
	{
		if (pWeather->enable) //����
		{
			switch(pWeather->effect)
			{
			case WEATHER_TYPE_FLASH:
				InitFlashEffect(pWeather->level);
				SetWorkMode(MODE_FLASH_VEIW);
				break;
			case WEATHER_TYPE_CLUED:
				InitCloudEffect(pWeather->level);
				SetWorkMode(MODE_CLUDE_VEIW);
				break;
			}
		}
		else  //�ر���Ч
		{
			DeleteEffect();
			SetWorkMode(MODE_TIMER);
		}

		respon = RET_OK;
	}
	else  //�����ָ��
	{
		respon = RET_NG;
	}
	
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//��������
//void SetMoonlingProc(void)
//{
//	MOONLING_s moonLightData;
//	
//	moonLightData.enable = PMoonLight->enable;
//	moonLightData.day = PMoonLight->day;
//	moonLightData.s_hour = PMoonLight->s_hour;
//	moonLightData.s_minute = PMoonLight->s_minute;
//	moonLightData.e_hour = PMoonLight->e_hour;
//	moonLightData.e_minute = PMoonLight->e_minute;
//	moonLightData.level = PMoonLight->level;
//	moonLightData.nc = PMoonLight->nc;
//	if ((moonLightData.day >= 1) && (moonLightData.day <= 28))
//	{
//		STM_FLASH_WRITE(FLASH_MOONLIGHT_ADDR + (moonLightData.day - 1)*sizeof(MOONLING_s),
//			(u16*)&moonLightData, sizeof(MOONLING_s));	//��������Ĭ������
//	}
//}

//�������
void MotorControlProcess(STRUCT_MOTOR *cmd)
{
	STRUCT_MOTOR cmd_buff;

	
	if (cmd->frame_mode == FRAME_MODE_GET)	//��ȡ����
	{
		STM_FLASH_READ(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));		//��ȡ����
		cmd_buff.frame_h = FRAME_START;
		cmd_buff.frame_cmd = FRAME_TYPE_MOTOR;
		cmd_buff.frame_mode = FRAME_MODE_GET;
		memcpy(&cmd_buff.enable, &motorInfo, sizeof(motorInfo));	//���Ƶ���������
		cmd_buff.frame_end = FRAME_END;
		ESP8266_SendDatas(current_linkID, (u8*)&cmd_buff, sizeof(cmd_buff));
	}
	else		//��������
	{
		memcpy(&motorInfo, &cmd->enable, sizeof(motorInfo));
		STM_FLASH_WRITE(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));	//��������

	}

}


//////////////////////////////////////////////////////////
//����LEDָ��
void LedCmdProc(void)
{
	int i;
	switch(pSystem->type)
	{
	//ʱ��
	case FRAME_TYPE_TIME: 
		switch(pTime->mode)
		{
		case FRAME_MODE_GET:
			GetTimeProc();  //ȡʱ��
			break;
		case FRAME_MODE_SET:
			SetTimeProc(); //��ʱ��
			break;
		}
		break;
	//����
	case FRAME_TYPE_DIMMER:

		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:
			GetDimmerProc();
			break;
		case FRAME_MODE_SET:
			if(USART_RECV_COUNT==20)		//�ж�һ������ȣ�����WiFi�ź���ʱ�����������������´������õ���
			{
				SetDimmerProc();
			}
			break;
		}
		break;

		//�ƹ���Ч
	case FRAME_TYPE_WEATHER:
		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:
			GetEffectProc();
			break;
		case FRAME_MODE_SET:
			switch (pSystem->command)
			{
			case WEATHER_TYPE_FLASH:
			case WEATHER_TYPE_CLUED:
				SetEffectProc();
				break;
			case WEATHER_TYPE_MOONLIGHT:
				
				//SetMoonlingProc();
				break;
			default:
				break;
			}
			break;
		}
		break;

		//��ʱ
	case FRAME_TYPE_TIMER:
		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:
			GetTimerProc();
			break;
		case FRAME_MODE_SET:
			SetTimerProc();
			break;
		}
		break;
	//ϵͳ
	case FRAME_TYPE_SYSTEM:
		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:  //��ȡϵͳ��Ϣ
			
//			for(i=0;i<USART_RECV_COUNT;i++)
//			{
//				SEGGER_RTT_printf(0,"%02x ",TX_RX_BUF[i]);
//			}
		
			GetSystemInfoProc();
			break;

		case FRAME_MODE_SET:  //����ϵͳ��Ϣ
			switch(pSystem->command)
			{
				//�ָ�Ĭ��
			case FRAME_CMD_DEFAULT:  
				SetDefaultProc();
				break;

				//�����̼�
			case FRAME_CMD_FIRMWARE: 
				SetUpdataModeProc(); //ת����������ģʽ
				break;

				//����ת��
			case FRAME_CMD_FAN:  
				SetFanSpeedProc();
				break;

				//�ͻ�����
			case FRAME_CMD_CUSTOM:  
				SetCustomCodeProc();
				break;

				//����ģʽ
			case FRAME_CMD_WORKMODE:  
				SetWorkModeProc();
				break;

				//���õ���·��
			case FRAME_CMD_PWM_NUMBER:  
				SetPwmNuberProc();
				break;

				//���ù���ָʾ��״̬:1:����0����
			case FRAME_CMD_LED_STA:  
				SetLedStateProc();
				break;

				//����WIFI����
			case FRAME_CMD_WIFI:  
				SetWifiStaConnProc();
				break;
			}
			break;

		case FRAME_TYPE_OTHTER:
			OtherProc();   //��������������ӵ�����
			break;
		
		}
		break;
	case FRAME_TYPE_JAP:
		ESP8266_ConnectWifiProc(TX_RX_BUF);
		sysFlag.Bits.WIFI_MODE = 1;			//WIFI STAģʽ
		wifiConnetFlag = 0;					//���WiFi��·�����ӶϿ� 
		STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //����
		//respon = RET_OK;
		//ESP8266_SendDatas(current_linkID, &respon, 1);
		os_dly_wait(2000);
		SoftReset();
		break;
	case FRAME_TYPE_LAP:
		ESP8266_ScanWifi();
		break;
	case FRAME_TYPE_MOTOR:
		MotorControlProcess(pMotor);
		break;

	}
}

//------------end of file----------------

