/************************************************************
Copyright (C), 2013-2015, SOSCI.Co.,Ltd.
@FileName: rtc.h
@Author  : 糊读虫
@Version : 1.0
@Date    : 2015-8-12
@Description: 读取温度
@Function List:
@History    : 
<author> <time> <version > <desc>
Lennon 2015/8/12 1.0 
***********************************************************/
#include "adc.h"
#include "temperature.h"

	/*
	计算公式：25度/（25度时的ADC值）*（4096－adc值）
	1个单位的ADC值对应温度为：25/(4096/2) = 0.0122度
	*/
//#define  TEMP_COVERT	(0.0122)	
#define  TEMP_NUM	100
const STRUCT_TEMP tlib[TEMP_NUM]=
{
	{ 0 , 3136  },
	{ 10 , 3100  },
	{ 20 , 3064  },
	{ 30 , 3027  },
	{ 40 , 2989  },
	{ 50 , 2950  },
	{ 60 , 2910  },
	{ 70 , 2870  },
	{ 80 , 2827  },
	{ 90 , 2784  },
	{ 100 , 2740  },
	{ 110 , 2694  },
	{ 120 , 2648  },
	{ 130 , 2602  },
	{ 140 , 2556  },
	{ 150 , 2510  },
	{ 160 , 2463  },
	{ 170 , 2417  },
	{ 180 , 2370  },
	{ 190 , 2324  },
	{ 200 , 2277  },
	{ 210 , 2231  },
	{ 220 , 2184  },
	{ 230 , 2138  },
	{ 240 , 2093  },
	{ 250 , 2048  },
	{ 260 , 2004  },
	{ 270 , 1960  },
	{ 280 , 1917  },
	{ 290 , 1875  },
	{ 300 , 1834  },
	{ 310 , 1792  },
	{ 320 , 1750  },
	{ 330 , 1708  },
	{ 340 , 1666  },
	{ 350 , 1623  },
	{ 360 , 1581  },
	{ 370 , 1539  },
	{ 380 , 1498  },
	{ 390 , 1457  },
	{ 400 , 1417  },
	{ 410 , 1379  },
	{ 420 , 1341  },
	{ 430 , 1306  },
	{ 440 , 1272  },
	{ 450 , 1240  },
	{ 460 , 1207  },
	{ 470 , 1175  },
	{ 480 , 1143  },
	{ 490 , 1112  },
	{ 500 , 1082  },
	{ 510 , 1052  },
	{ 520 , 1022  },
	{ 530 , 993  },
	{ 540 , 965  },
	{ 550 , 938  },
	{ 560 , 911  },
	{ 570 , 885  },
	{ 580 , 859  },
	{ 590 , 835  },
	{ 600 , 811  },
	{ 610 , 788  },
	{ 620 , 765  },
	{ 630 , 743  },
	{ 640 , 721  },
	{ 650 , 699  },
	{ 660 , 678  },
	{ 670 , 657  },
	{ 680 , 637  },
	{ 690 , 617  },
	{ 700 , 599  },
	{ 710 , 581  },
	{ 720 , 563  },
	{ 730 , 547  },
	{ 740 , 532  },
	{ 750 , 517  },
	{ 760 , 504  },
	{ 770 , 491  },
	{ 780 , 478  },
	{ 790 , 465  },
	{ 800 , 453  },
	{ 810 , 441  },
	{ 820 , 429  },
	{ 830 , 417  },
	{ 840 , 406  },
	{ 850 , 394  },
	{ 860 , 383  },
	{ 870 , 372  },
	{ 880 , 361  },
	{ 890 , 351  },
	{ 900 , 340  },
	{ 910 , 332  },
	{ 920 , 324  },
	{ 930 , 316  },
	{ 940 , 308  },
	{ 950 , 299  },
	{ 960 , 290  },
	{ 970 , 282  },
	{ 980 , 273  },
	{ 990 , 265  }
};


/*------------------------------------
ADC扫描热敏电阻
-------------------------------------*/
short GetTemperature(void)
{
	u8 i;
	u16 temp;
	temp = Get_Adc(ADC_Channel_9);
	//temp = GetADC(ADC_CH1);

//	temp = (u16)(TEMP_COVERT*10.0*(4096-temp));
//	OLED_ShowNum(50,16,temp,5,16);
//	OLED_Refresh_Gram();

	//优化:要改成2分查找法
	for (i=0;i<TEMP_NUM;i++)
	{
		if (tlib[i].adcVal <= temp)
		{
			return tlib[i].temp;
		}
	}
	return tlib[0].temp; //返回0度
}

