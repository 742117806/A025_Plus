#ifndef _RTX_OS_H
#define _RTX_OS_H
#include <RTL.h>

extern OS_TID t_taskClock;
extern OS_TID t_taskUartProc;
extern OS_TID t_taskDisplay;
extern OS_TID t_taskPwmChage;

#define OS_UPDATA_OLED() os_evt_set(T_MSG_UPDATA_OLED,t_taskDisplay)

//t_taskDisplay任务消息
#define T_MSG_MENU_EN		0x0002//(0x0001<<2)
#define T_MSG_MENU_DESABLE	0x0004//(0x0001<<3)
#define T_MSG_UPDATA_OLED	0x0008//(0x0001<<4)
#define T_MSG_SECOND_CLOCK	0x0010//(0x0001<<5)
#define T_MSG_UPDATA_GUI	0x0020//(0x0001<<6)

#define TMR_DISP	TMR1
#define TMR_CLOCK	TMR2
#define TMR_OTHER	TMR3

#endif
