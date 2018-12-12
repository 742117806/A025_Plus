/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: key.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 按键驱动 
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "adc.h"
#include "delay.h"
#include "key.h"
#include "oled.h"
#include "stmflash.h"
#include "system.h"
#include "EC11_Key.h"

/*按键硬件设置*/
#define  ADC_K1		adc_key[0]
#define  ADC_K2		adc_key[1]
#define  ADC_K3		adc_key[2]
#define  ADC_K4		adc_key[3]

#define	 ADC_KEY_EXIT	 	ADC_K1
#define	 ADC_KEY_ENTER		ADC_K2
#define	 ADC_KEY_UP	 		ADC_K4
#define	 ADC_KEY_DOWN	  	ADC_K3

#define  ADC_OFFSET 	200
#define  VF_ADC_OFFSET	300

#define VF_NUM	3

#define ADC_KEY_CH	ADC_Channel_8


KeyStatue_ENUM	KeyStatue;  //按键状态
KeyValue_ENUM nowKeyValue;  //当前的键值

u16 adc_key[4]={ADC_K1_A,ADC_K2_A,ADC_K3_A,ADC_K4_A};

u16 CheckADC(u16 *keyAdc,u8 *str);


/*------------------------------------
触摸校准
-------------------------------------*/
void TouchCheck(void)
{
	u16 temp;
	u8 *str;
	
	if (Get_Adc(ADC_KEY_CH) > ADC_K1_A-200)return; //检测到ADC值小于1100才进入校准，否则返回
	
	OLED_Clear_Ram();

	//重新初始化
	ADC_K1 = ADC_K1_A;
	ADC_K2 = ADC_K2_A;
	ADC_K3 = ADC_K3_A;
	ADC_K4 = ADC_K4_A;

	//EXIT 
	str = "Press ESC key  ";
	while(!CheckADC(&ADC_KEY_EXIT,str));

	str = "Press OK key   ";
	while(!CheckADC(&ADC_KEY_ENTER,str));

	str = "Press '-' key  ";
	while(!CheckADC(&ADC_KEY_DOWN,str));

	str = "Press '+' key  ";
	while(!CheckADC(&ADC_KEY_UP,str));
	
	//校准完后显示所有按键的值
	OLED_Clear_Ram();
	OLED_ShowNum(1,0,ADC_KEY_EXIT,5,16);
	OLED_ShowNum(60,0,ADC_KEY_ENTER,5,16);
	OLED_ShowNum(1,16,ADC_KEY_DOWN,5,16);
	OLED_ShowNum(60,16,ADC_KEY_UP,5,16);
	OLED_Refresh_Gram();
	SavaTouchParam();
	
	temp = 100;
	while(temp--)
	{
		if (KeyDriver() == KEY_EXIT)break; //按ESC键立即退出
		delay_ms(100);
	}
}

u16 abs_adc(u16 num1,u16 num2)
{
	return num1 > num2?num1-num2:num2-num1;
}

//按键校准
u16 CheckADC(u16 *keyAdc,u8 *str)
{
	u16 cnt;
	u8 cntOK=0;
	u16 KeyVal;
	u16 PreKey;

	OLED_ShowString(0,0,str);
	OLED_ShowString(0,16,"x   num:     ");
	OLED_Refresh_Gram();
	delay_ms(1000);
	delay_ms(1000);

	PreKey = Get_Adc(ADC_KEY_CH);
	//printf("pre:%d \n",PreKey);
	for(cnt=VF_NUM;cnt>0;cnt--)
	{
		KeyVal = Get_Adc(ADC_KEY_CH);  //取得ADC值
		OLED_ShowNum(60,16,KeyVal,5,16);
		OLED_ShowNum(1,16,cnt,1,16);
		OLED_Refresh_Gram();
		
		if((KeyVal> *keyAdc-VF_ADC_OFFSET) && (KeyVal < *keyAdc+VF_ADC_OFFSET))
		{
			if (abs_adc(PreKey,KeyVal) < 30)
			{
				cntOK++;
			}
			else cntOK = 0;
		}

		delay_ms(500);
	}

	if (cntOK >=VF_NUM)
	{
		*keyAdc = PreKey;
		OLED_ShowString(0,0,"      OK        ");
		OLED_ShowNum(60,16,*keyAdc,5,16);
		OLED_Refresh_Gram();
		delay_ms(1000);
		delay_ms(1000);
		return 1;
		
	}
	else
	{
		OLED_ShowString(0,0,"      NG        ");
		OLED_ShowNum(60,16,*keyAdc,5,16);
		OLED_Refresh_Gram();
		delay_ms(1000);
		delay_ms(1000);
		return 0;
	}
}

/*------------------------------------
保存触摸数据
-------------------------------------*/
void SavaTouchParam(void)
{
	STM_FLASH_WRITE(FLASH_TOUCH_ADDR,(u16*)&adc_key,sizeof(adc_key)); //保存PWM信息
}

/*------------------------------------
读取触摸数据
-------------------------------------*/
void ReadTouchParam(void)
{
	STM_FLASH_READ(FLASH_TOUCH_ADDR,(u16*)&adc_key,sizeof(adc_key));//PWM信息
	//printf("key adc:%d,%d,%d,%d \n",adc_key[0],adc_key[1],adc_key[2],adc_key[3]);
}

/*------------------------------------
ADC扫描按键
-------------------------------------*/
KeyValue_ENUM KeyDriver(void)
{
#if 0
	u16 KeyVal;
	
//	KeyVal = Get_Adc_Average(ADC_Channel_1,5); 
	KeyVal = Get_Adc(ADC_KEY_CH);

 //	OLED_ShowNum(50,16,KeyVal,5,16);
 //	OLED_Refresh_Gram();

	if((KeyVal> ADC_KEY_EXIT-ADC_OFFSET) && (KeyVal < ADC_KEY_EXIT+ADC_OFFSET)) return KEY_EXIT;
	else if((KeyVal> ADC_KEY_ENTER-ADC_OFFSET) && (KeyVal < ADC_KEY_ENTER+ADC_OFFSET)) return KEY_ENTER;
	else if((KeyVal> ADC_KEY_UP-ADC_OFFSET) && (KeyVal < ADC_KEY_UP+ADC_OFFSET)) return KEY_UP;
	else if((KeyVal> ADC_KEY_DOWN-ADC_OFFSET) && (KeyVal < ADC_KEY_DOWN+ADC_OFFSET)) return KEY_DOWN;
	else return KEY_OFF;
#else
	extern u8 ec11_LeftCnt;
	extern u8 ec11_RightCnt;
	static u16 pressCnt = 0;
	static u8  pressStep = 0;
	static u8 EC11_KeyPressCnt = 0;

	if (EC11_KEY == 0)
	{
		EC11_KeyPressCnt++;
		if (EC11_KeyPressCnt > 200)
		{
			EC11_KeyPressCnt = 0;
			return KEY_RESET;
		}
	}
	else
	{
		EC11_KeyPressCnt = 0;
	}
	switch (pressStep)
	{
	case 0:
		if (EC11_KEY == 0)
			pressStep = 1;		//按下按键
		break;
	case 1:
		pressCnt++;
		if (EC11_KEY == 1)
		{
			pressStep = 2;//松开按键
		}

		if (pressCnt > 15)
		{
			pressStep = 3;
			return KEY_ENTER;
		}
		break;
	case 2:
		pressCnt ++;
		if (pressCnt < 15)
		{
			if (EC11_KEY == 0)
			{
				pressStep = 3;
				return KEY_EXIT;
			}
		}
		else
		{
			pressStep = 3;
			return KEY_ENTER;
		}
		break;
	case 3:
		if (EC11_KEY == 1)		//按键松开
		{
			pressStep = 0;
			pressCnt = 0;
		}
		break;
	default:
		
		break;
	}
	



	if(ec11_LeftCnt == 1)
	{
		ec11_LeftCnt = 0;
		return KEY_UP;
	}
	else if (ec11_RightCnt == 1)
	{
		ec11_RightCnt = 0;
		return KEY_DOWN;
	}
	else return KEY_OFF;
#endif
}

//设置按键方向
void SetKeyStaue(KeyStatue_ENUM sta) 
{
	KeyStatue = sta;
}

/********************************************************************************
 * Function Name  : 按键值处理
 * Description    : 加减数值
 * Input&Output   : None        
 *******************************************************************************/
void add_dec_u8(u8 *Value,u8 ValueMax,u8 ValueMin,u8 loop)
{
	switch(KeyStatue)
	{
	case keyAdd_flag: //加键
		KeyStatue = keyClear_flag;   
		if (loop)  //循环
		{
			if(++*Value > ValueMax) *Value = ValueMin;
		}
		else  //不循环
		{
			if(++*Value > ValueMax) *Value = ValueMax;
		}
		break;
	case keyDec_flag: //减1
		KeyStatue = keyClear_flag;
		if (loop)
		{
			if (*Value == 0)*Value = ValueMax;
			else if(--*Value == ValueMin-1) *Value = ValueMax;
		}
		else
		{
			if (*Value == 0)*Value = ValueMin;
			else if(--*Value == ValueMin-1) *Value = ValueMin;
		}
		break;
	default:
		break;
	}
}

//选择两个数中的一个
void Choose(u8 *Value,u8 numA,u8 numB)
{
	switch(KeyStatue)
	{
	case keyAdd_flag:
	case keyDec_flag:
		KeyStatue = keyClear_flag;   
		if (*Value != numA)*Value = numA;
		else *Value = numB;
		break;
	}
}

extern void ChangeMenu(KeyValue_ENUM key);

/*-----------------------------
按键扫描任务
返回：	0：无按键
		1：单击
		2：长按
------------------------------*/
KeyValue_ENUM TaskKeyScan() 
{   
	static KeyValue_ENUM preKeyValue; //上次的键值
	static u16 KeyDownCnt;  //按下计数

	nowKeyValue  = KeyDriver();

	if ((nowKeyValue != preKeyValue)) //当前键值与上次不同，按下不同的键
	{
		preKeyValue = nowKeyValue;
		KeyDownCnt = 0;
		if(nowKeyValue != KEY_OFF)return nowKeyValue;//ChangeMenu(nowKeyValue);
	}
	else if (nowKeyValue != KEY_OFF) //相同的键
	{	
		KeyDownCnt++;
		if (KeyDownCnt > KEY_DOWN_CNT) //长按
		{
			KeyDownCnt = KEY_DOWN_CNT - KEY_SPEED;
			//ChangeMenu(nowKeyValue);
			return nowKeyValue;
		}
	}
	return KEY_OFF;
}

/*-----------end of file-----------------*/
