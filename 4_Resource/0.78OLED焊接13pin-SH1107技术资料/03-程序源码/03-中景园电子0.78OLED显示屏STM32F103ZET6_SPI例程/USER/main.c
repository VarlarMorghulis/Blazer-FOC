//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//中景园电子
//店铺地址：http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  文 件 名   : main.c
//  版 本 号   : v2.0
//  作    者   : HuangKai
//  生成日期   : 2014-0101
//  最近修改   : 
//  功能描述   : OLED 4接口演示例程(51系列)
//              说明: 
//              ----------------------------------------------------------------
//              GND    电源地
//              VCC  接5V或3.3v电源
//              D0   接PG12（SCL）
//              D1   接PD5（SDA）
//              RES  接PD4
//              DC   接PD15
//              CS   接PD1 
//              ----------------------------------------------------------------
// 修改历史   :
// 日    期   : 
// 作    者   : HuangKai
// 修改内容   : 创建文件
//版权所有，盗版必究。
//Copyright(C) 中景园电子2014/3/16
//All rights reserved
//******************************************************************************/

#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "led.h"
int main(void)
{
	u8 t=' ';
	delay_init();
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	LED0=0;
	while(1)
	{
		OLED_ShowPicture(0,0,80,128,BMP1,1);
		OLED_Refresh();
		delay_ms(500);
		OLED_Clear();
		OLED_ShowChinese(16,0,0,16,1);//中
		OLED_ShowChinese(32,0,1,16,1);//景
		OLED_ShowChinese(48,0,2,16,1);//园
		OLED_ShowChinese(8,16,3,16,1);//电
		OLED_ShowChinese(24,16,4,16,1);//子
		OLED_ShowChinese(40,16,5,16,1);//科
		OLED_ShowChinese(56,16,6,16,1);//技
		OLED_ShowString(0,32,"ZHONGJINGYUAN",12,1);
		OLED_ShowString(0,48,"2014/05/01",16,1);
		OLED_ShowString(0,64,"ASCII:",16,1);  
		OLED_ShowString(0,80,"CODE:",16,1);
		OLED_ShowChar(48,64,t,16,1);//显示ASCII字符	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(40,80,t,3,16,1);
		OLED_Refresh();
		delay_ms(500);
		OLED_Clear();
		OLED_ShowChinese(0,8,0,16,1);  //16*16 中
	  OLED_ShowChinese(16,4,0,24,1); //24*24 中
		OLED_ShowChinese(40,0,0,32,1);//32*32 中
	  OLED_ShowChinese(0,32,0,64,1); //64*64 中
		OLED_ShowString(0,110,"AB",8,1);//6*8 “ABC”
		OLED_ShowString(14,108,"CD",12,1);//6*12 “ABC”
	  OLED_ShowString(28,106,"EF",16,1);//8*16 “ABC”
		OLED_ShowString(46,102,"GH",24,1);//12*24 “ABC”
	  OLED_Refresh();
		delay_ms(500);
		OLED_ScrollDisplay(11,4,1);
	}
}

