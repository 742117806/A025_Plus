#ifndef _SYSTEM_H
#define _SYSTEM_H

#include "stm32f10x.h "

/*-------------编译设置----------------*/
/*************
1、把中断向量 SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET 改为SCB->VTOR = FLASH_BASE | APP_VECT_TAB_OFFSET;
2、编译选项IROM的起始地址是：0x8000000 + APP_VECT_TAB_OFFSET
3、SIZE大小是0x10000 - APP_VECT_TAB_OFFSET
4、BootLoader的APP1地址也要做相应修改 
***************/
#define APP_VECT_TAB_OFFSET		0x2800	//0x200*12=0x1800, 留12K的空间给BOOTLOADER

//STM32 地址
#define FLASH_PAGE				1024
#define FLASH_END_ADDR			0x08010000
#define FLASH_SYS_ADDR			(FLASH_END_ADDR-FLASH_PAGE*1) //保存首次运行  
#define FLASH_LANGUAGE_ADDR		(FLASH_SYS_ADDR + 0x02)   //保存语言
#define FLASH_CUSTOM_CODE		(FLASH_SYS_ADDR + 0x04) //保存客户标识代码
#define FLASH_TOUCH_ADDR		(FLASH_SYS_ADDR + 0x10)  //保存触摸信息
#define FLASH_MANUAL_PWM		(FLASH_SYS_ADDR + 0x20)  //保存手动信息
#define FLASH_EASY_TIMER		(FLASH_SYS_ADDR + 0x30)  //保存EASY调光信息
#define FLASH_SYSFLAG_ADDR		(FLASH_SYS_ADDR + 0x50)  //保存系统数据信息
#define FLASH_EFFECT_ADDR		(FLASH_SYS_ADDR + 0x60)  //保存灯光效果
#define FLASH_TIMER_ADDR		(FLASH_SYS_ADDR + 0xA0)  //保存定时信息
//#define FLASH_MOONLIGHT_ADDR	(FLASH_SYS_ADDR + 0xA0+240)  //保存月相信息
#define FLASH_MOTOR_INFO_ADDR	(FLASH_SYS_ADDR + 0xA0+240)	//电机相关信息

//#define FLASH_MOONLIGHT_ADDR   (FLASH_SYS_ADDR + 0xA0)  //保存月相信息
//#define FLASH_TIMER_ADDR   (FLASH_SYS_ADDR + 0x170)  //保存定时信息

//W25Q64 Flash空间分配
#define W25QXX_USER_ADDR		0x000FF000	//
#define W25QXX_CUSTOM_CODE		(W25QXX_USER_ADDR+0)//客户代码 16位
#define W25QXX_PWM_NUM			(W25QXX_USER_ADDR+2)//PWM数量
#define W25QXX_MAC				(W25QXX_USER_ADDR+4)//MAC地址
#define W25QXX_CHIP_CODE		(W25QXX_USER_ADDR+32)//芯片代码
#define W25QXX_LOGO				(W25QXX_USER_ADDR+64)//LOGO数据

#define W25QXX_FONT_ADDR		0x00100000  //字体地址

//工作模式
#define MODE_TIMER		0x00	//定时模式
#define MODE_MUNUAL		0x01	//手动模式
//#define MODE_WEATHER	0x02	//天气 云彩//闪电模式

#define MODE_FLASH		0x02
#define MODE_CLUDE		0x03
#define MODE_FLASH_VEIW	0x04
#define MODE_CLUDE_VEIW	0x05
#define	MODE_MOONLIGHT	0x06	//月光模式
#define MODE_MOONLIGHT_VEIW	0x07//月光模式预览
#define MODE_UPDATA		0x80 //升级APP

//#define MODE_WEATHER_VIEW	0x07	//天气 云彩//闪电模式
#define MODE_POWER_OFF		0x08	//关灯模式
#define MODE_PREVIEW	0x10	//定时预览

//0x02闪电   0x03多云    0x04闪电预览   0x05 多云预览

//软件版本
#define V1	20
#define V2	96
extern const u8 version[2];

#define SUNRISE_TIME	120	//日出、日落时间,单位：分
#define PWM_SPEED_TIMER		20	//定时模式下PWM变化速度
#define PWM_SPEED_FLASH		30	//定时模式下PWM变化速度
#define PWM_SPEED_DIMMER	1	//手动模式下PWM变化速度	
#define PWM_SPEED_PREVIEW	1	//定时预览下PWM变化速度	

//升级类型 
#define UPDATA_APP	0  //升级软件
#define UPDATA_FONT	1  //升级字库

//客户订制,客户,涉及开机LOGO和WIFI名称
//#define USER_SANRISE  1	 //1:Sanrise;
//#define USER_AQUALED  1	 //2:Aqualed;
//#define USER_ARCADIA  1	 //3:Arcadia;
//#define USER_SPECTRA	1	//4:Spectra
#define USER_AQUARIUM	1	//5:Spectra
#define LOGO_EN		0  //是否启用开机LOGO


#endif
