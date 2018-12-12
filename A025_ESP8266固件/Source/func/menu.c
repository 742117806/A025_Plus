/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: CMDPROC.C
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 菜单操作
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
//*目录结构体定义
//*-------------------------------
typedef struct 	MenuItem						//结构体名称
{	
	unsigned char 		MenuCount;         //当前层节点数、每层菜单能显示的条目数
	unsigned char 		*DisplayString;    //指向菜单标题的字符串
	unsigned char		*DisplayStringCN;  //中文
	struct MenuItem 	*ChildrenMenus;//指向当前菜单的下级菜单
	struct MenuItem 	*ParentMenus;  //指向当前菜单的上级菜单
	void (*SubFunc)();         //指向当前状态应该执行的功能函数
}StructMenuItem; 

const StructMenuItem mSystemSet[8];	//声明一下
const StructMenuItem mEasyTimerSet[5];	//声明一下
const StructMenuItem mEffect[2];	//声明一下
const StructMenuItem mWifiConfig[3];
//const StructMenuItem mWifiMenu[2];
StructMenuItem NullMenu; //空菜单

#if 0
#define N_MAIN	5
const StructMenuItem mMainMenu[N_MAIN] =      //主菜单
{
	{N_MAIN,"1.Brightness   ","1.当前亮度     ",(StructMenuItem *)&NullMenu,&NullMenu,&ShowCurrentPwmMenu}, //当前亮度
	{N_MAIN,"2.Power On/Off ","2.开/关灯      ",(StructMenuItem *)&NullMenu,&NullMenu,	&LedOnOff}, // 开/关灯
	{N_MAIN,"3.Dimmer       ","3.手动         ",(StructMenuItem *)&NullMenu,&NullMenu,&ManualSetMenu}, //配光
	{N_MAIN,"4.Timer        ","4.定时         ",(StructMenuItem *)&mEasyTimerSet,&NullMenu,&NullFucn},//定时
//	{N_MAIN,"5.Effect       ","5.系统设置     ",(StructMenuItem *)&mEffect,&NullMenu,&NullFucn},//场景
	{N_MAIN,"5.System       ","5.系统设置     ",(StructMenuItem *)&mSystemSet,&NullMenu,&NullFucn}, //系统设置
//	{N_MAIN,"6.Help         ",(StructMenuItem *)&NullMenu,&NullMenu,	&Help}, //帮助
};
#else
#define N_MAIN	2
const StructMenuItem mMainMenu[N_MAIN] =      //主菜单
{
	{ N_MAIN, "  Wifi  ", "1.Wifi模式", (StructMenuItem *)&NullMenu, &NullMenu, &WifiApModeConfig },
	{ N_MAIN, " Manual ", "2.手动模式", (StructMenuItem *)&NullMenu, &NullMenu, &ManualSetMenu }, // 配光
};
#endif

//const StructMenuItem mWifiMenu[2] =
//{
//	{ 2, "Wifi mode", "", &NullMenu, (StructMenuItem *)&mMainMenu, &WifiApModeConfig },
//	{ 2, "Wifi Reset", "", &NullMenu, &NullMenu, &ESP8266_LoadDefault },
//};
const StructMenuItem mSystemSet[8] =	   //系统设置菜单
{
	{8,"1.System time  ","1.日期/时间    ",&NullMenu,(StructMenuItem*)mMainMenu,&SetDataTime},
	{8,"2.Wifi Config  ","2.Wifi 配置    ",(StructMenuItem*)&mWifiConfig,(StructMenuItem*)mMainMenu,&NullFucn},
	{8,"3.Load Default ","3.恢复默认设置 ",&NullMenu,(StructMenuItem*)mMainMenu,&LoadDefault},
	{8,"4.Temperature  ","4.灯具温度     ",&NullMenu,(StructMenuItem*)mMainMenu,&TempFan},
	{8,"5.Pilot light  ","5.指示灯       ",&NullMenu,(StructMenuItem*)mMainMenu,&PilotLight},
	{8,"6.Language/语言","6.Language/语言",&NullMenu,(StructMenuItem*)mMainMenu,&Language},
	{8,"7.Version      ","7.软件版本     ",&NullMenu,(StructMenuItem*)mMainMenu,&Version},
	{8,"8.User Logo    ","8.查看产品LOGO ",&NullMenu,(StructMenuItem*)mMainMenu,&DisplayUserLogo},
};

const StructMenuItem mEasyTimerSet[5] =	   //定时,日出日落
{
	{5,"1.Sunrise time ","1.日出时间     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SunriseSet},//日出时间
	{5,"2.Sunset  time ","2.日落时间     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SundownSet},//日落时间
	{5,"3.Sunlight     ","3.白天亮度     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&SunlightSet},//白天亮度
	{5,"4.Moonlight    ","4.月光亮度     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&MoonlightSet},//月光亮度
	{5,"5.Time preview ","5.定时预览     ",(StructMenuItem*)&NullMenu,(StructMenuItem*)mMainMenu,&PreviewTimer},//预览
};

const StructMenuItem mEffect[2] =	   //情景模式
{
	{2,"1.Lightning    ","1.闪电         ",&NullMenu,(StructMenuItem*)mMainMenu,&ViewFlashEffect},
	{2,"2.Clouds       ","2.云彩         ",&NullMenu,(StructMenuItem*)mMainMenu,&ViewCloudEffect},
};

const StructMenuItem mWifiConfig[3] =	   //WIFI配置
{
	{3,"1.Wifi AP mode ","1.Wifi热点模式 ",&NullMenu,(StructMenuItem*)mSystemSet,&WifiApModeConfig},
	{3,"2.Wifi STA mode","2.接入网络模式 ",&NullMenu,(StructMenuItem*)mSystemSet,&WifiStaModeConfig},
	{3,"3.Wifi info    ","3.查看Wifi状态 ",&NullMenu,(StructMenuItem*)mMainMenu,&WifiInfo},
};

StructMenuItem *MenuPoint = (StructMenuItem*)mMainMenu; //初始菜单

//菜单信息结构体
typedef struct _MenuInfo
{
	u8	DisplayStart; 			       //显示菜单的开始位置
	u8 	UserChoose; 				  //用户的选择
	u8 	DisplayPoint; 				  //显示菜单的编号
	u8 	MaxItems;  					//菜单的数量
//	u8 	Option;						//保存用户的菜单编号
}MenuInfo;

MenuInfo currentMenu ={0,0,0,0}; //菜单信息
MenuInfo lastMenu ={0,0,0,0}; //前一个菜单信息

SubChoose subInfo = {0,0}; //记录按键操作，传递给函数


MenuFlag menuFlag;//菜单标记


#define MENU_LOOP	0 //1:启用菜单循环

//函数声明
void ShowMenu(void);

//*-------------------------------
//*菜单执行函数
//*-------------------------------
void ChangeMenu(KeyValue_ENUM key)
{
	//判断是不是第一次从桌面进入菜单，
	if (!menuFlag.Bits.isInMenu)
	{
		if (key == KEY_ENTER) //只有按ENTER键才能进入菜单
		{
			menuFlag.Bits.isInMenu = 1;
			menuFlag.Bits.updataMenu = 1;
			//os_evt_set(T_MSG_MENU_DESABLE,t_taskDisplay); //不显示桌面
			SetClockFun(NullFucn); //不显示桌面
			currentMenu.DisplayStart = 0;
			currentMenu.UserChoose = 0;
		}
	}
	else
	{
		if (!menuFlag.Bits.isMenu)  //按键操作在菜单界面
		{
			currentMenu.MaxItems = MenuPoint[0].MenuCount;        //获取当前菜单的条目数
			switch (key)
			{
			case  KEY_DOWN:  //菜单加
#if MENU_LOOP  
				//菜单循环显示
				if (currentMenu.UserChoose == 0)currentMenu.UserChoose = currentMenu.MaxItems - 1;  //用户菜单循环选择
				else currentMenu.UserChoose--;	
#else          
				//菜单不循环显示
				if (currentMenu.UserChoose > 0)currentMenu.UserChoose--;//菜单往下减
				
				//显示位置
				if (currentMenu.DisplayStart > currentMenu.UserChoose) 
				{
					currentMenu.DisplayStart = currentMenu.UserChoose;
				}
#endif
				menuFlag.Bits.updataMenu = 1;
				break;
			case KEY_UP: //菜单减
#if MENU_LOOP
				//菜单循环显示
				if (currentMenu.UserChoose >= currentMenu.MaxItems-1)currentMenu.UserChoose = 0;
				else currentMenu.UserChoose++;

#else
				//菜单不循环显示
				if (currentMenu.UserChoose < currentMenu.MaxItems-1)currentMenu.UserChoose++;

				//显示位置
				if ((currentMenu.DisplayStart+LCD1602MaxDisplaYLine-1) < currentMenu.UserChoose) //计算开始显示位置
				{
					currentMenu.DisplayStart++;
				}			
#endif					
				menuFlag.Bits.updataMenu = 1;
				break;
			case KEY_ENTER:
				if ((MenuPoint[currentMenu.UserChoose].SubFunc) != NullFucn)
				{
					menuFlag.Bits.isMenu = 1;  //转到函数功能
					subInfo.step = 0;
					(*MenuPoint[currentMenu.UserChoose].SubFunc)();  //运行函数功能
				}
				else if (MenuPoint[currentMenu.UserChoose].ChildrenMenus != &NullMenu)
				{
					MenuPoint 	 = MenuPoint[currentMenu.UserChoose].ChildrenMenus;  //指向子菜单
					
					//保存当前的菜单选择及位置
					lastMenu.UserChoose = currentMenu.UserChoose;
					lastMenu.DisplayStart = currentMenu.DisplayStart;
					lastMenu.DisplayPoint = currentMenu.DisplayPoint;

					//设置子菜单的选择及位置
					currentMenu.UserChoose 	 = 0;
					currentMenu.DisplayStart	= 0;
					menuFlag.Bits.updataMenu = 1;
				}
				break;
			case KEY_EXIT:
				if (MenuPoint[0].ParentMenus != &NullMenu)//如果存在父菜单
				{
					MenuPoint 	 = MenuPoint[0].ParentMenus; 			 //指向父菜单

					//恢复先前的菜单选择及位置
					currentMenu.DisplayStart = lastMenu.DisplayStart;
					currentMenu.DisplayPoint = lastMenu.DisplayPoint;
					currentMenu.UserChoose = lastMenu.UserChoose;

					menuFlag.Bits.updataMenu = 1;
				}
				else  //已经到菜单顶层
				{
					menuFlag.Bits.isInMenu = 0;
					menuFlag.Bits.updataMenu = 0;
					menuFlag.Bits.runFunc = 0;
					//os_evt_set(T_MSG_MENU_EN,t_taskDisplay); //显示桌面
					//os_evt_set(T_MSG_SECOND_CLOCK,t_taskDisplay); 
					SetClockFun(Desktop);
				}
				break;
			}
		}
		else  //按键操作在功能界面
		{
			switch(key)
			{
			case  KEY_EXIT:  //退出功能，回到菜单界面
				ReturnToMenu();
				menuFlag.Bits.isMenu = 0;
				menuFlag.Bits.updataMenu = 1; //更新菜单
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
	if(menuFlag.Bits.updataMenu)  //更新菜单显示
	{
		menuFlag.Bits.updataMenu = 0;
		
#if MENU_LOOP
		//*菜单循环处理
	 	if ((currentMenu.UserChoose < currentMenu.DisplayStart) || (currentMenu.UserChoose >= (currentMenu.DisplayStart + 1 )))
 		{
		 	currentMenu.DisplayStart = currentMenu.UserChoose;
	 	}
#endif
		OLED_Clear_Ram();
		ShowMenu();
	}
	else if (menuFlag.Bits.runFunc) //执行函数功能
	{
		menuFlag.Bits.runFunc = 0;
		if ((MenuPoint[currentMenu.UserChoose].SubFunc) != NullFucn)
		{
			(*MenuPoint[currentMenu.UserChoose].SubFunc)();  //运行指定功能
		}
	}
}

//*-------------------------------
//*显示菜单函数
//*-------------------------------
void ShowMenu(void)
{
	u8 i = 0;
	currentMenu.MaxItems = MenuPoint[0].MenuCount;        //获取当前菜单的条目数
	currentMenu.DisplayPoint = currentMenu.DisplayStart;
	if (currentMenu.MaxItems > LCD1602MaxDisplaYLine)//如果大于能显示的菜单数做处理
	{	  
		for (i = 0;i < LCD1602MaxDisplaYLine;i++)
		{
			//IF_EN()	ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayString); //英文
			IF_EN()	ShowStringCN(i*8, 0, MenuPoint[currentMenu.DisplayPoint].DisplayString); //英文
			IF_CN() ShowStringCN(1,i,MenuPoint[currentMenu.DisplayPoint].DisplayStringCN);  //中文
			if ((currentMenu.DisplayPoint) == (currentMenu.UserChoose))
			{
				//ShowString(0,i,">");   //写小箭头
				ShowString(i*8+3, 1, "V");   //写小箭头
			}
			else
			{
				//ShowString(0,i,' ');	  //写空格
				ShowString(i*8+3, 1, " ");	  //写空格
			}
			currentMenu.DisplayPoint += 1;   //字符串偏移量+1
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
				//ShowString(0,i,">");   //写小箭头
				ShowString(i*8+3, 1, "V");   //写小箭头
			}
			else
			{
				//ShowString(0,i,' ');	  //写空格
				ShowString(i*8+3, 1, " ");	  //写空格
			}
			currentMenu.DisplayPoint += 1;	//字符串偏移量+1
			if ((currentMenu.DisplayPoint) == (currentMenu.MaxItems))
			{
				currentMenu.DisplayPoint = 0;
			}
		}
	}
	OS_UPDATA_OLED();
}
 
//复位菜单
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
