/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : ������
@Version : 1.0
@Date    : 2015-8-12
@Description: �˵�����
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/

#include "menu.h"
#include "oled.h"
#include "my_type.h"
#include "func.h"
#include "func.h"
#include "rtx_os.h"
#include "gbk_text.h"
#include "ESP8266_07S.h"
//*-------------------------------
//*Ŀ¼�ṹ�嶨��
//*-------------------------------
typedef struct 	MenuItem						//�ṹ������
{	
	unsigned char 		MenuCount;         //��ǰ��ڵ�����ÿ��˵�����ʾ����Ŀ��
	unsigned char 		*DisplayString;    //ָ��˵�������ַ���
	unsigned char		*DisplayStringCN;  //����
	struct MenuItem 	*ChildrenMenus;//ָ��ǰ�˵����¼��˵�
	struct MenuItem 	*ParentMenus;  //ָ��ǰ�˵����ϼ��˵�
	void (*SubFunc)();         //ָ��ǰ״̬Ӧ��ִ�еĹ��ܺ���
}StructMenuItem; 

const StructMenuItem mSystemSet[8];	//����һ��
const StructMenuItem mEasyTimerSet[5];	//����һ��
const StructMenuItem mEffect[2];	//����һ��
const StructMenuItem mWifiConfig[3];
//const StructMenuItem mWifiMenu[2];
StructMenuItem NullMenu; //�ղ˵�

#if 0
#define N_MAIN	5
const StructMenuItem mMainMenu[N_MAIN] =      //���˵�
{
	{N_MAIN,"1.Brightness   ","1.��ǰ����     ",(StructMenuItem *)&NullMenu,&NullMenu,&ShowCurrentPwmMenu}, //��ǰ����
	{N_MAIN,"2.Power On/Off ","2.��/�ص�      ",(StructMenuItem *)&NullMenu,&NullMenu,	&LedOnOff}, // ��/�ص�
	{N_MAIN,"3.Dimmer       ","3.�ֶ�         ",(StructMenuItem *)&NullMenu,&NullMenu,&ManualSetMenu}, //���
	{N_MAIN,"4.Timer        ","4.��ʱ         ",(StructMenuItem *)&mEasyTimerSet,&NullMenu,&NullFucn},//��ʱ
//	{N_MAIN,"5.Effect       ","5.ϵͳ����     ",(StructMenuItem *)&mEffect,&NullMenu,&NullFucn},//����
	{N_MAIN,"5.System       ","5.ϵͳ����     ",(StructMenuItem *)&mSystemSet,&NullMenu,&NullFucn}, //ϵͳ����
//	{N_MAIN,"6.Help         ",(StructMenuItem *)&NullMenu,&NullMenu,	&Help}, //����
};
#else
#define N_MAIN	2
const StructMenuItem mMainMenu[N_MAIN] =      //���˵�
{
	{ N_MAIN, "  Wifi  ", "1.Wifiģʽ", (StructMenuItem *)&NullMenu, &NullMenu, &WifiApModeConfig },
	{ N_MAIN, " Manual ", "2.�ֶ�ģʽ", (StructMenuItem *)&NullMenu, &NullMenu, &ManualSetMenu }, // ���
};
#endif

//const StructMenuItem mWifiMenu[2] =
//{
//	{ 2, "Wifi mode", "", &NullMenu, (StructMenuItem *)&mMainMenu, &WifiApModeConfig },
//	{ 2, "Wifi Reset", "", &NullMenu, &NullMenu, &ESP8266_LoadDefault },
//};
const StructMenuItem mSystemSet[8] =	   //ϵͳ���ò˵�
{
	{8,"1.System time  ","1.����/ʱ��    ",&NullMenu,(StructMenuItem*)mMainMenu,&SetDataTime},
	{8,"2.Wifi Config  ","2.Wifi ����    ",(StructMenuItem*)&mWifiConfig,(StructMenuItem*)mMainMenu,&NullFucn},
	{8,"3.Load Default ","3.�ָ�Ĭ������ ",&NullMenu,(StructMenuItem*)mMainMenu,&LoadDefault},
	{8,"4.Temperature  ","4.�ƾ��¶�     ",&NullMenu,(StructMenuItem*)mMainMenu,&TempFan},
	{8,"5.Pilot light  ","5.ָʾ��       ",&NullMenu,(StructMenuItem*)mMainMenu,&PilotLight},
	{8,"6.Language/����","6.Language/����",&NullMenu,(StructMenuItem*)mMainMenu,&Language},
	{8,"7.Version      ","7.����汾     ",&NullMenu,(StructMenuItem*)mMainMenu,&Version},
	{8,"8.User Logo    ","8.�鿴��ƷLOGO ",&NullMenu,(StructMenuItem*)mMainMenu,&DisplayUserLogo},
};

const StructMenuItem mEasyTimerSet[5] =	   //��ʱ,�ճ�����
{
	{5,"1.Sunrise time ","1.�ճ�ʱ��     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SunriseSet},//�ճ�ʱ��
	{5,"2.Sunset  time ","2.����ʱ��     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SundownSet},//����ʱ��
	{5,"3.Sunlight     ","3.��������     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SunlightSet},//��������
	{5,"4.Moonlight    ","4.�¹�����     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&MoonlightSet},//�¹�����
	{5,"5.Time preview ","5.��ʱԤ��     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&PreviewTimer},//Ԥ��
};

const StructMenuItem mEffect[2] =	   //�龰ģʽ
{
	{2,"1.Lightning    ","1.����         ",&NullMenu,(StructMenuItem*)mMainMenu,&ViewFlashEffect},
	{2,"2.Clouds       ","2.�Ʋ�         ",&NullMenu,(StructMenuItem*)mMainMenu,&ViewCloudEffect},
};

const StructMenuItem mWifiConfig[3] =	   //WIFI����
{
	{3,"1.Wifi AP mode ","1.Wifi�ȵ�ģʽ ",&NullMenu,(StructMenuItem*)mSystemSet,&WifiApModeConfig},
	{3,"2.Wifi STA mode","2.��������ģʽ ",&NullMenu,(StructMenuItem*)mSystemSet,&WifiStaModeConfig},
	{3,"3.Wifi info    ","3.�鿴Wifi״̬ ",&NullMenu,(StructMenuItem*)mMainMenu,&WifiInfo},
};

StructMenuItem *MenuPoint = (StructMenuItem*)mMainMenu; //��ʼ�˵�

//�˵���Ϣ�ṹ��
typedef struct _MenuInfo
{
	u8	DisplayStart; 			       //��ʾ�˵��Ŀ�ʼλ��
	u8 	UserChoose; 				  //�û���ѡ��
	u8 	DisplayPoint; 				  //��ʾ�˵��ı��
	u8 	MaxItems;  					//�˵�������
//	u8 	Option;						//�����û��Ĳ˵����
}MenuInfo;

MenuInfo currentMenu ={0,0,0,0}; //�˵���Ϣ
MenuInfo lastMenu ={0,0,0,0}; //ǰһ���˵���Ϣ

SubChoose subInfo = {0,0}; //��¼�������������ݸ�����


MenuFlag menuFlag;//�˵����


#define MENU_LOOP	0 //1:���ò˵�ѭ��

//��������
void ShowMenu(void);

//*-------------------------------
//*�˵�ִ�к���
//*-------------------------------
void ChangeMenu(KeyValue_ENUM key)
{
	//�ж��ǲ��ǵ�һ�δ��������˵���
	if (!menuFlag.Bits.isInMenu)
	{
		if (key == KEY_ENTER) //ֻ�а�ENTER�����ܽ���˵�
		{
			menuFlag.Bits.isInMenu = 1;
			menuFlag.Bits.updataMenu = 1;
			//os_evt_set(T_MSG_MENU_DESABLE,t_taskDisplay); //����ʾ����
			SetClockFun(NullFucn); //����ʾ����
			currentMenu.DisplayStart = 0;
			currentMenu.UserChoose = 0;
		}
	}
	else
	{
		if (!menuFlag.Bits.isMenu)  //���������ڲ˵�����
		{
			currentMenu.MaxItems = MenuPoint[0].MenuCount;        //��ȡ��ǰ�˵�����Ŀ��
			switch (key)
			{
			case  KEY_DOWN:  //�˵���
#if MENU_LOOP  
				//�˵�ѭ����ʾ
				if (currentMenu.UserChoose == 0)currentMenu.UserChoose = currentMenu.MaxItems - 1;  //�û��˵�ѭ��ѡ��
				else currentMenu.UserChoose--;	
#else          
				//�˵���ѭ����ʾ
				if (currentMenu.UserChoose > 0)currentMenu.UserChoose--;//�˵����¼�
				
				//��ʾλ��
				if (currentMenu.DisplayStart > currentMenu.UserChoose) 
				{
					currentMenu.DisplayStart = currentMenu.UserChoose;
				}
#endif
				menuFlag.Bits.updataMenu = 1;
				break;
			case KEY_UP: //�˵���
#if MENU_LOOP
				//�˵�ѭ����ʾ
				if (currentMenu.UserChoose >= currentMenu.MaxItems-1)currentMenu.UserChoose = 0;
				else currentMenu.UserChoose++;

#else
				//�˵���ѭ����ʾ
				if (currentMenu.UserChoose < currentMenu.MaxItems-1)currentMenu.UserChoose++;

				//��ʾλ��
				if ((currentMenu.DisplayStart+LCD1602MaxDisplaYLine-1) < currentMenu.UserChoose) //���㿪ʼ��ʾλ��
				{
					currentMenu.DisplayStart++;
				}			
#endif					
				menuFlag.Bits.updataMenu = 1;
				break;
			case KEY_ENTER:
				if ((MenuPoint[currentMenu.UserChoose].SubFunc) != NullFucn)
				{
					menuFlag.Bits.isMenu = 1;  //ת����������
					subInfo.step = 0;
					(*MenuPoint[currentMenu.UserChoose].SubFunc)();  //���к�������
				}
				else if (MenuPoint[currentMenu.UserChoose].ChildrenMenus != &NullMenu)
				{
					MenuPoint 	 = MenuPoint[currentMenu.UserChoose].ChildrenMenus;  //ָ���Ӳ˵�
					
					//���浱ǰ�Ĳ˵�ѡ��λ��
					lastMenu.UserChoose = currentMenu.UserChoose;
					lastMenu.DisplayStart = currentMenu.DisplayStart;
					lastMenu.DisplayPoint = currentMenu.DisplayPoint;

					//�����Ӳ˵���ѡ��λ��
					currentMenu.UserChoose 	 = 0;
					currentMenu.DisplayStart	= 0;
					menuFlag.Bits.updataMenu = 1;
				}
				break;
			case KEY_EXIT:
				if (MenuPoint[0].ParentMenus != &NullMenu)//������ڸ��˵�
				{
					MenuPoint 	 = MenuPoint[0].ParentMenus; 			 //ָ�򸸲˵�

					//�ָ���ǰ�Ĳ˵�ѡ��λ��
					currentMenu.DisplayStart = lastMenu.DisplayStart;
					currentMenu.DisplayPoint = lastMenu.DisplayPoint;
					currentMenu.UserChoose = lastMenu.UserChoose;

					menuFlag.Bits.updataMenu = 1;
				}
				else  //�Ѿ����˵�����
				{
					menuFlag.Bits.isInMenu = 0;
					menuFlag.Bits.updataMenu = 0;
					menuFlag.Bits.runFunc = 0;
					//os_evt_set(T_MSG_MENU_EN,t_taskDisplay); //��ʾ����
					//os_evt_set(T_MSG_SECOND_CLOCK,t_taskDisplay); 
					SetClockFun(Desktop);
				}
				break;
			}
		}
		else  //���������ڹ��ܽ���
		{
			switch(key)
			{
			case  KEY_EXIT:  //�˳����ܣ��ص��˵�����
				ReturnToMenu();
				menuFlag.Bits.isMenu = 0;
				menuFlag.Bits.updataMenu = 1; //���²˵�
				break;

			case  KEY_DOWN:
				//KeyStatue = keyDec_flag;
				SetKeyStaue(keyDec_flag);
				menuFlag.Bits.runFunc = 1;
				break;
			case KEY_UP:
				menuFlag.Bits.runFunc = 1;
				//KeyStatue = keyAdd_flag;
				SetKeyStaue(keyAdd_flag);
				break;
			case KEY_ENTER:
				//KeyStatue = keyAdd_flag;
				SetKeyStaue(keyAdd_flag);
				add_dec_u8(&subInfo.step,subInfo.maxSetp,0,1);
				menuFlag.Bits.runFunc = 1;
				break;
			}
		}
	}


//==================================================
	if(menuFlag.Bits.updataMenu)  //���²˵���ʾ
	{
		menuFlag.Bits.updataMenu = 0;
		
#if MENU_LOOP
		//*�˵�ѭ������
	 	if ((currentMenu.UserChoose < currentMenu.DisplayStart) || (currentMenu.UserChoose >= (currentMenu.DisplayStart + 1 )))
 		{
		 	currentMenu.DisplayStart = currentMenu.UserChoose;
	 	}
#endif
		OLED_Clear_Ram();
		ShowMenu();
	}
	else if (menuFlag.Bits.runFunc) //ִ�к�������
	{
		menuFlag.Bits.runFunc = 0;
		if ((MenuPoint[currentMenu.UserChoose].SubFunc) != NullFucn)
		{
			(*MenuPoint[currentMenu.UserChoose].SubFunc)();  //����ָ������
		}
	}
}

//*-------------------------------
//*��ʾ�˵�����
//*-------------------------------
void ShowMenu(void)
{
	u8 i = 0;
	currentMenu.MaxItems = MenuPoint[0].MenuCount;        //��ȡ��ǰ�˵�����Ŀ��
	currentMenu.DisplayPoint = currentMenu.DisplayStart;
	if (currentMenu.MaxItems > LCD1602MaxDisplaYLine)//�����������ʾ�Ĳ˵���������
	{	  
		for (i = 0;i < LCD1602MaxDisplaYLine;i++)
		{
			//IF_EN()	ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayString); //Ӣ��
			IF_EN()	ShowStringCN(i*8, 0, MenuPoint[currentMenu.DisplayPoint].DisplayString); //Ӣ��
			IF_CN() ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayStringCN);  //����
			if ((currentMenu.DisplayPoint) == (currentMenu.UserChoose))
			{
				//ShowString(0,i,">");   //дС��ͷ
				ShowString(i*8+3, 1, "V");   //дС��ͷ
			}
			else
			{
				//ShowString(0,i,' ');	  //д�ո�
				ShowString(i*8+3, 1, " ");	  //д�ո�
			}
			currentMenu.DisplayPoint += 1;   //�ַ���ƫ����+1
			if ((currentMenu.DisplayPoint) == (currentMenu.MaxItems))
			{
				currentMenu.DisplayPoint = 0;
				break;
			}
		}
	}
	else
	{
		for (i = 0;i < LCD1602MaxDisplaYLine;i++){
			//IF_EN() ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayString);
			IF_EN() ShowStringCN(i*8, 0, MenuPoint[currentMenu.DisplayPoint].DisplayString);
			IF_CN() ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayStringCN);
			if ((currentMenu.DisplayPoint) == (currentMenu.UserChoose))
			{
				//ShowString(0,i,">");   //дС��ͷ
				ShowString(i*8+3, 1, "V");   //дС��ͷ
			}
			else
			{
				//ShowString(0,i,' ');	  //д�ո�
				ShowString(i*8+3, 1, " ");	  //д�ո�
			}
			currentMenu.DisplayPoint += 1;	//�ַ���ƫ����+1
			if ((currentMenu.DisplayPoint) == (currentMenu.MaxItems))
			{
				currentMenu.DisplayPoint = 0;
			}
		}
	}
	OS_UPDATA_OLED();
}
 
//��λ�˵�
void ResetMenuToDestop(void)
{
	if (menuFlag.Bits.isInMenu)
	{
		menuFlag.Bits.isInMenu = 0;
		menuFlag.Bits.updataMenu = 0;
		menuFlag.Bits.runFunc = 0;
		SetClockFun(Desktop);
	}
}
