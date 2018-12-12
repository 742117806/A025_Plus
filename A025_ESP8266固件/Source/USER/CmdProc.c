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


u8 respon; //回应数据

//清空内存
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

//设置时间
void SetTimeProc(void)
{
	RTC_Set(
		2000+pTime->year, //年份以2000年为基准往上加
		pTime->month,
		pTime->day,
		pTime->hour,
		pTime->minute,
		pTime->second
		); //设置时间

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//读取时间
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

//读取系统信息
void GetSystemInfoProc()
{
	STRUCT_SYSTEM sys;
	u16 custom_cdoe; //
	unsigned int mac;
	extern u8 CPU_GUID[12]; //96位芯片唯一标识
	ClearRam((u8*)&sys,sizeof(sys));

	custom_cdoe =GetCustomCode(); //读取客户代码

	sys.start = FRAME_START;
	sys.type = FRAME_TYPE_SYSTEM;
	sys.mode = FRAME_MODE_GET;
	sys.command = FRAME_CMD_EXIST;

	sys.para[SYSTEM_TEMP] = sysInfo.temperature;//GetTemperature()/10; //温度;
	sys.para[SYSTEM_VERSION1] = version[0]; //软件版本1
	sys.para[SYSTEM_VERSION2] = version[1]; //软件版本2
	sys.para[SYSTEM_EXIST]  = 0x5A;	//存在确认,这里总是0x5A
	sys.para[SYSTEM_FANSPEED] = GetFanSpeed(); //风扇转速
	sys.para[SYSTEM_CUSTOM_CODE1] = custom_cdoe>>8;  //客户代码
	sys.para[SYSTEM_CUSTOM_CODE2] = (u8)custom_cdoe;
	sys.para[SYSTEM_MODE] = sysInfo.workmode; //当前模式
	sys.para[SYSTEM_PWM_NUM] = GetPwmNumber(); //调光路数
	sys.para[SYSTEM_TIMER_INDEX] = sysInfo.indexTimer+1; //当前定时点 从1~ 12

	sys.para[SYSTEM_LED_STATE] = sysFlag.Bits.LED_sta; //指示灯状态

	//取WFIF 模块MAC的后面2位数十六进制
	sscanf((const char*)CPU_GUID+4, "%x", &mac);
	
	sys.para[SYSTEM_GUID0] = 0; 
	sys.para[SYSTEM_GUID1] = ((mac>>8)&0x00ff);
	sys.para[SYSTEM_GUID2] = (mac&0x00ff);

	sys.end = FRAME_END;
	
	
	ESP8266_SendDatas(current_linkID, (u8*)&sys, sizeof(sys));

}


//转到升级固件模式
void SetUpdataModeProc(void)
{
	respon = RET_OK;
	//uart_sendbuf(&respon,1);	
	ESP8266_SendDatas(current_linkID, &respon, 1);
	SetUartRxType(USART_TYYE_DATA); 
	USART_RX_STA = 0;
	sysInfo.updataType = pSystem->para[SYSTEM_MODE]; //指定升级类型:
}


//设置PWM值
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

	SetWorkMode(MODE_MUNUAL); //转到手动模式

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//设置PWM值
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


//设置FAN
void SetFanSpeedProc(void)
{
	if (pSystem->para[SYSTEM_FANSPEED] > 100) //风扇值大于100就回到自动温控模式
	{
		sysInfo.MunualFan = 0;
	}
	else
	{
		SetFanSpeed(pSystem->para[SYSTEM_FANSPEED]);
		sysInfo.MunualFan = 1; //设置手动转速
	}	
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}


//设置客户标识代码
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

//恢复到默认设置
void SetDefaultProc(void)
{
	SystemLoadDefault();
	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//设置工作模式
void SetWorkModeProc(void)
{
	SetWorkMode(pSystem->para[SYSTEM_MODE]); 

	if (sysInfo.workmode == MODE_POWER_OFF)//关灯模式
	{
		SetPwmA(0,0,0,0,0,0);
	}

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//设置调光路数
void SetPwmNuberProc(void)
{
	u8 number;
	number = pSystem->para[SYSTEM_PWM_NUM];
	if (number == 3 || number == 6)//只能是3路或6路
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

//设置工作指示灯状态
void SetLedStateProc(void)
{
	if (pSystem->para[SYSTEM_LED_STATE])sysFlag.Bits.LED_sta = 1;
	else sysFlag.Bits.LED_sta = 0;

	 STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //保存

	respon = RET_OK;
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//设置WIFI STA连接
/*
para[0]:要设置的WIFI数据,1=WIFI名称;2=WIFI密码;3=加密方式
para[1]~para[14]:设置的数据 
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

//取定时设置
void GetTimerProc()
{
	STRUCT_TIMER tmr;
	TIMER tm_step;
	u8 addr;
	u8 len;
		
	ClearRam((u8*)&tmr,sizeof(tmr));
	//STM32的FLASH是16位对齐，所以读取的字节数必须是2字节的倍数
	//风险：因为实际申请内存是9字节，而实际读取了10个字节，可能把其它变量改了
	len = sizeof(TIMER);  //sizeof结果是9，要转成10，
	if (len%2)len=len+1;

	//上位机的定时时段序号是1,2,3,4,5...，单片机以0,1,2,开始，要转换一下
	//如果给定的index是0,默认读取定时1
	if (pTimer->index >0 && pTimer->index <=PRGM_STEP)//检查是否合法
	{
		addr =sizeof(TIMER)* (pTimer->index - 1);

		STM_FLASH_READ(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//读取PWM信息

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
		tmr.count = GetMaxTimerStep(); //获取定时点数量
		tmr.end  =	FRAME_END;

		//uart_sendbuf((u8*)&tmr,sizeof(tmr));
		ESP8266_SendDatas(current_linkID, (u8*)&tmr, sizeof(tmr));
	}
	else
	{
		//返回定时点数量
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
		tmr.count = GetMaxTimerStep(); //获取定时点数量
		tmr.end  =	FRAME_END;
		//uart_sendbuf((u8*)&tmr,sizeof(tmr));
		ESP8266_SendDatas(current_linkID, (u8*)&tmr, sizeof(tmr));
	}

}

//设置定时
void SetTimerProc(void)
{
	TIMER tm_step;
	u16 addr;
	u8 len;
	u8 i;
	u8 index; //索引
	u8 count; //定时总数

	//STM32的FLASH是16位对齐，所以读取的字节数必须是2字节的倍数
	//风险：因为实际申请内存是9字节，而实际读取了10个字节，可能把其它变量改了
	len = sizeof(TIMER);  //sizeof结果是9，要转成10，
	if (len%2)len=len+1;

	index = pTimer->index;
	count = pTimer->count;

	//检查定时时段是否合法 
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
		STM_FLASH_WRITE(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//写入PWM信息

		if (index == count) //如果是最后一个定时点,把剩下的定时全部关闭
		{
			for(i=count;i<PRGM_STEP;i++)
			{
				tm_step.enable = 0;
				addr =sizeof(TIMER)* i;
				STM_FLASH_WRITE(FLASH_TIMER_ADDR+addr,(u16*)&tm_step,len);//写入PWM信息
			}
		}

		SetWorkMode(MODE_TIMER);
		respon = RET_OK;
	}
	else if(pTimer->index == 0) //index=0 就是定时预览
	{
		if (pTimer->enable)  //显示定时预览
		{
			SetClockFun(NullFucn);
			SetPreveiwTime(pTimer->hour,pTimer->minute);
			PreviewTimer(); //	
			ResetMenuToDestop(); //菜单返回到桌面
		}
		else   //关闭定时预览
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

//取灯光特效
void GetEffectProc(void)
{
	STRUCT_WEATHER effect;
	WEATHER eff_step;
	u8 addr;
	u8 len;

	ClearRam((u8*)&effect,sizeof(effect));
	//STM32的FLASH是16位对齐，所以读取的字节数必须是2字节的倍数
	//风险：因为实际申请内存是9字节，而实际读取了10个字节，可能把其它变量改了
	len = sizeof(WEATHER);  //sizeof结果是9，要转成10，
	if (len%2)len=len+1;

	//上位机的定时时段序号是1,2,3,4,5...，单片机以0,1,2,开始，要转换一下
	//如果给定的index是0,默认读取定时1
	if (pWeather->index >0 && pWeather->index <=WEATHER_SETP)//检查是否合法
	{
		addr =sizeof(WEATHER)* (pWeather->index - 1);

		STM_FLASH_READ(FLASH_EFFECT_ADDR+addr,(u16*)&eff_step,len);//读取PWM信息

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
			
		effect.count = GetMaxEffectStep(); //获取特效数量
		effect.end  =	FRAME_END;
	}
	else
	{
		//返回定时点数量
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

		effect.count = GetMaxEffectStep(); //获取特效数量
		effect.end  =	FRAME_END;
	}
	//uart_sendbuf((u8*)&effect,sizeof(effect));
	ESP8266_SendDatas(current_linkID, (u8*)&effect, sizeof(effect));
}

//设置灯光特效
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

	//pWeather->level = 100 - pWeather->level;  //要转换一下

	if(index >0 && index <=WEATHER_SETP)  //保存特效
	{
		len = sizeof(WEATHER);  //sizeof结果是9，要转成10，
		if (len%2)len=len+1;

		effect_step.enable = pWeather->enable;
		effect_step.level = pWeather->level;
		effect_step.effect = pWeather->effect;
		effect_step.s_hour = pWeather->s_hour;
		effect_step.s_minute = pWeather->s_minute;
		effect_step.e_hour = pWeather->e_hour;
		effect_step.e_minute = pWeather->e_minute;
		
		addr =sizeof(WEATHER)*(index-1);
		STM_FLASH_WRITE(FLASH_EFFECT_ADDR+addr,(u16*)&effect_step,len);//写入天气

		if (index == count) //如果是最后一个定时点,把剩下的定时全部关闭
		{
			for(i=count;i<WEATHER_SETP;i++)
			{
				effect_step.enable = 0;
				addr =sizeof(WEATHER)* i;
				STM_FLASH_WRITE(FLASH_EFFECT_ADDR+addr,(u16*)&effect_step,len);//写入PWM信息
			}
		}

		SetWorkMode(MODE_TIMER);
		respon = RET_OK;
	}
	else if (pWeather->index == 0) //预览
	{
		if (pWeather->enable) //开启
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
		else  //关闭特效
		{
			DeleteEffect();
			SetWorkMode(MODE_TIMER);
		}

		respon = RET_OK;
	}
	else  //错误的指令
	{
		respon = RET_NG;
	}
	
	//uart_sendbuf(&respon,1);
	ESP8266_SendDatas(current_linkID, &respon, 1);
}

//月相设置
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
//			(u16*)&moonLightData, sizeof(MOONLING_s));	//保存月相默认数据
//	}
//}

//电机控制
void MotorControlProcess(STRUCT_MOTOR *cmd)
{
	STRUCT_MOTOR cmd_buff;

	
	if (cmd->frame_mode == FRAME_MODE_GET)	//获取数据
	{
		STM_FLASH_READ(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));		//读取数据
		cmd_buff.frame_h = FRAME_START;
		cmd_buff.frame_cmd = FRAME_TYPE_MOTOR;
		cmd_buff.frame_mode = FRAME_MODE_GET;
		memcpy(&cmd_buff.enable, &motorInfo, sizeof(motorInfo));	//复制电机相关数据
		cmd_buff.frame_end = FRAME_END;
		ESP8266_SendDatas(current_linkID, (u8*)&cmd_buff, sizeof(cmd_buff));
	}
	else		//设置数据
	{
		memcpy(&motorInfo, &cmd->enable, sizeof(motorInfo));
		STM_FLASH_WRITE(FLASH_MOTOR_INFO_ADDR, (u16*)&motorInfo, sizeof(motorInfo));	//保存数据

	}

}


//////////////////////////////////////////////////////////
//处理LED指令
void LedCmdProc(void)
{
	int i;
	switch(pSystem->type)
	{
	//时间
	case FRAME_TYPE_TIME: 
		switch(pTime->mode)
		{
		case FRAME_MODE_GET:
			GetTimeProc();  //取时间
			break;
		case FRAME_MODE_SET:
			SetTimeProc(); //置时间
			break;
		}
		break;
	//调光
	case FRAME_TYPE_DIMMER:

		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:
			GetDimmerProc();
			break;
		case FRAME_MODE_SET:
			if(USART_RECV_COUNT==20)		//判断一下命令长度，由于WiFi信号弱时候结束不完整的命令导致错误设置调光
			{
				SetDimmerProc();
			}
			break;
		}
		break;

		//灯光特效
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

		//定时
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
	//系统
	case FRAME_TYPE_SYSTEM:
		switch(pSystem->mode)
		{
		case FRAME_MODE_GET:  //获取系统信息
			
//			for(i=0;i<USART_RECV_COUNT;i++)
//			{
//				SEGGER_RTT_printf(0,"%02x ",TX_RX_BUF[i]);
//			}
		
			GetSystemInfoProc();
			break;

		case FRAME_MODE_SET:  //设置系统信息
			switch(pSystem->command)
			{
				//恢复默认
			case FRAME_CMD_DEFAULT:  
				SetDefaultProc();
				break;

				//升级固件
			case FRAME_CMD_FIRMWARE: 
				SetUpdataModeProc(); //转到数据升级模式
				break;

				//风扇转速
			case FRAME_CMD_FAN:  
				SetFanSpeedProc();
				break;

				//客户代码
			case FRAME_CMD_CUSTOM:  
				SetCustomCodeProc();
				break;

				//工作模式
			case FRAME_CMD_WORKMODE:  
				SetWorkModeProc();
				break;

				//设置调光路数
			case FRAME_CMD_PWM_NUMBER:  
				SetPwmNuberProc();
				break;

				//设置工作指示灯状态:1:开，0：关
			case FRAME_CMD_LED_STA:  
				SetLedStateProc();
				break;

				//配置WIFI连网
			case FRAME_CMD_WIFI:  
				SetWifiStaConnProc();
				break;
			}
			break;

		case FRAME_TYPE_OTHTER:
			OtherProc();   //处理其它额外添加的命令
			break;
		
		}
		break;
	case FRAME_TYPE_JAP:
		ESP8266_ConnectWifiProc(TX_RX_BUF);
		sysFlag.Bits.WIFI_MODE = 1;			//WIFI STA模式
		wifiConnetFlag = 0;					//标记WiFi与路由连接断开 
		STM_FLASH_WRITE(FLASH_SYSFLAG_ADDR,(u16*)&sysFlag,sizeof(sysFlag)); //保存
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

