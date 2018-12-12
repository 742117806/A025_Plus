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

#ifndef H_CMD_PROC_H
#define H_CMD_PROC_H

#include "sys.h"

//#define	FRAME_START		241	//0xF1 帧开始
#define	FRAME_START		242	//0xF2 帧开始
#define	FRAME_END		243	//0xF3 帧结束
#define	FRAME_MODE_SET	250	//0xFA 置数据
#define	FRAME_MODE_GET	252	//0xFC 取数据

//数据OK
#ifndef RET_OK
#define RET_OK		245		//0xF5
#endif

//数据NG
#ifndef RET_NG
#define RET_NG		246		//0xF6
#endif


#define	FRAME_TYPE_SYSTEM	225	//0xE1 系统指令
#define FRAME_TYPE_DIMMER	226	//0xE2 调光
#define	FRAME_TYPE_TIME		227	//0xE3 时间
#define	FRAME_TYPE_TIMER	228	//0xE4 定时
#define	FRAME_TYPE_WEATHER	229	//0xE5 天气
#define	FRAME_TYPE_OTHTER	232	//0xE8 其它指令
#define	FRAME_TYPE_JAP		233	//0xE9 配置家庭网络
#define	FRAME_TYPE_LAP		234	//0xEA 获取WIFI列表
#define FRAME_TYPE_MOTOR	235	//0xEB 电机控制

#define	FRAME_CMD_CLOUD		209	//0xD1 云彩
#define	FRAME_CMD_LIGHTNING	210	//0xD2 闪电

#define	FRAME_CMD_DEFAULT	211	//0xD3 默认设置
//#define	FRAME_CMD_POWER		212	//0xD4 LED开关
#define	FRAME_CMD_FIRMWARE	213	//0xD5 升级固件
#define	FRAME_CMD_TEMPERATURE	214 //0xD6 温度
#define	FRAME_CMD_FAN		215 //0xD7 风扇转速
#define	FRAME_CMD_VERSION	216 //0xD8 固件版本
#define	FRAME_CMD_EXIST 	217	//0xD9	存在确认
#define	FRAME_CMD_CUSTOM	218	//0xDA	//客户标识代码
#define	FRAME_CMD_WORKMODE	219	//0xDB	//工作模式
#define	FRAME_CMD_PWM_NUMBER	220	//0xDC	//PWM数量
#define	FRAME_CMD_LED_STA		221	//0xDD	//指示灯状态
#define FRAME_CMD_WIFI		222 //0xDE //配置WIFI

//指令长度固定为20个字节

//时间结构体
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 year;
	u8 month;
	u8 day;
	u8 hour;
	u8 minute;
	u8 second;
	u8 nc[10];  //保留,未使用
	u8 end;
}STRUCT_TIME; 

//定时结构体
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 index;	//定时序号
	u8 enable;
	u8 hour;
	u8 minute;
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc[5];  //保留,未使用
	u8 count; //定时数量
	u8 end;
}STRUCT_TIMER;

//天气结构体
typedef struct {
	u8 start;	//
	u8 type;	//
	u8 mode;
	u8 effect;	//特效类型
	u8 enable;	//
	u8 index;	//序号:1~4, 如果为0,则执行预览
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //强弱等级
	u8 nc[7];	//保留,未使用
	u8 count;  //特效数量
	u8 end;	//
}STRUCT_WEATHER;

//月相命令结构体
typedef struct {
	u8 start;	//
	u8 type;	//
	u8 mode;
	u8 effect;	//特效类型
	u8 enable;	//
	u8 day;		//月相日期（1-28号）
	u8 s_hour;	//
	u8 s_minute;	//
	u8 e_hour;	//
	u8 e_minute;	//
	u8 level;  //强弱等级
	u8 nc;	//保留,未使用
	u8 end;	//
}STRUCT_MOONLIGH;

//调光结构体
typedef struct {
	u8 start;
	u8 type;
	u8 mode;
	u8 pwm1;
	u8 pwm2;
	u8 pwm3;
	u8 pwm4;
	u8 pwm5;
	u8 pwm6;
	u8 nc[10];
	u8 end;
}STRUCT_DIMMER;

//系统结构体
typedef struct {
	u8 start;
	u8 type;	//系统
	u8 mode;	//命令:GET\SET
	u8 command;	
	u8 para[15];	//15个参数: 工作模式(1)；风扇转速(1)；温度(1)；软件版本(2)；客户代码(2)；调光路数(1)；
	u8 end;
}STRUCT_SYSTEM;


//电机控制数据设置结构体
typedef struct
{
	u8 frame_h;	//帧头
	u8 frame_cmd;	//命令（是设置或者获取什么信息的识别字节）
	u8 frame_mode;	//（相对APP来说）是设置数据（FA）还是获取数据（FC）
	u8 enable;
	u8 s_hour1;
	u8 s_minute1;
	u8 e_hour1;
	u8 e_minute1;
	u8 s_hour2;
	u8 s_minute2;
	u8 e_hour2;
	u8 e_minute2;
	u8 s_hour3;
	u8 s_minute3;
	u8 e_hour3;
	u8 e_minute3;
	u8 angle_h;
	u8 angle_l;
	u8 speed;
	u8 frame_end;
}STRUCT_MOTOR;

#define SYSTEM_MODE		0  //工作模式
#define SYSTEM_FANSPEED	1	//风扇转速
#define SYSTEM_TEMP		2  //温度
#define SYSTEM_VERSION1	3  //软件版本1
#define SYSTEM_VERSION2	4  //软件版本2
#define SYSTEM_CUSTOM_CODE1		5  //客户代码1
#define SYSTEM_CUSTOM_CODE2		6  //客户代码2
#define SYSTEM_PWM_NUM	7		//调光路数
#define SYSTEM_TIMER_INDEX	8		//当前定时点
#define SYSTEM_LED_STATE	9		//当前定时点

#define SYSTEM_GUID0	11
#define SYSTEM_GUID1	12
#define SYSTEM_GUID2	13
#define SYSTEM_EXIST	14  //存在

#define TX_RX_BUF	USART_RX_BUF //接收/发送缓冲

#define  pTime 			((STRUCT_TIME *)TX_RX_BUF) //时间	
#define  pTimer 		((STRUCT_TIMER *)TX_RX_BUF) //定时
#define  pWeather 		((STRUCT_WEATHER *)TX_RX_BUF) //天气
#define  pSystem		((STRUCT_SYSTEM *)TX_RX_BUF) //系统
#define  pDimmer		((STRUCT_DIMMER *)TX_RX_BUF) //调光
#define  PMoonLight		((STRUCT_MOONLIGH *)TX_RX_BUF)//月相
#define  pMotor			((STRUCT_MOTOR *)TX_RX_BUF)//电机	

void SetMoonlingProc(void);
void LedCmdProc(void);
void ClearRam(u8 *addr,u16 len);
#endif

