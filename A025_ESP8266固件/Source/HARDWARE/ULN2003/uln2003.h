#ifndef __ULN2003_H
#define __ULN2003_H	 
#include "sys.h"
#include "rtc.h"
#include "func.h"
#define MOTO_SETP_NUM	8					//步进电机的步数
#define MotorData GPIOB                    //步进电机控制接口定义
#define MotorA			PAout(6)
#define MotorB			PAout(8)
#define MotorC			PBout(10)
#define MotorD			PBout(11)

#define MOTO_KEY1	GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键	
#define MOTO_KEY2	GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)//读取按键	


#define LED_MOTOR_STEP_MAX		5800

extern u16 motor_current_step;
extern u16 motor_set_step;

void MotorRunPresse(void);
u8 MotorIsEffective(u8 cur_hour, u8 cur_min);
void MotorBackOrigin(void);
uint16_t GetMotorCurrentStep(void);
void MotorStop(void);
void MotorStart(uint8_t flag,u16 arr);
void MotorGoBack(void);
void MotorGoTo(uint16_t step);
void MotorInit(void);
uint16_t SetMotoCurrentStep(uint16_t step);
uint32_t GetMotorRunTimer(void);
void MotorAutoGoBack(_calendar_obj time);
uint16_t MotorGotoCurrentPoint(uint16_t step_sum,_calendar_obj calendar,MotorInfo_s motor_info);
void MotorRest(void);
void MotorRun(uint16_t speed,uint8_t dir);
void MotorStepRefresh(void);
uint32_t  GetMotorRunSetp(void);
u32 GetMotorMissSec(_calendar_obj time);
u16 GetMotorMissStep(void);
#endif
