//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//�о�԰����
//���̵�ַ��http://shop73023976.taobao.com/?spm=2013.1.0.0.M4PqC2
//
//  �� �� ��   : main.c
//  �� �� ��   : v2.0
//  ��    ��   : HuangKai
//  ��������   : 2014-0101
//  ����޸�   : 
//  ��������   : OLED 4�ӿ���ʾ����(51ϵ��)
//              ˵��: 
//              ----------------------------------------------------------------
//              GND    ��Դ��
//              VCC  ��5V��3.3v��Դ
//              D0   ��PG12��SCL��
//              D1   ��PD5��SDA��
//              RES  ��PD4
//              DC   ��PD15
//              CS   ��PD1 
//              ----------------------------------------------------------------
// �޸���ʷ   :
// ��    ��   : 
// ��    ��   : HuangKai
// �޸�����   : �����ļ�
//��Ȩ���У�����ؾ���
//Copyright(C) �о�԰����2014/3/16
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
	OLED_ColorTurn(0);//0������ʾ��1 ��ɫ��ʾ
  OLED_DisplayTurn(0);//0������ʾ 1 ��Ļ��ת��ʾ
	LED0=0;
	while(1)
	{
		OLED_ShowPicture(0,0,80,128,BMP1,1);
		OLED_Refresh();
		delay_ms(500);
		OLED_Clear();
		OLED_ShowChinese(16,0,0,16,1);//��
		OLED_ShowChinese(32,0,1,16,1);//��
		OLED_ShowChinese(48,0,2,16,1);//԰
		OLED_ShowChinese(8,16,3,16,1);//��
		OLED_ShowChinese(24,16,4,16,1);//��
		OLED_ShowChinese(40,16,5,16,1);//��
		OLED_ShowChinese(56,16,6,16,1);//��
		OLED_ShowString(0,32,"ZHONGJINGYUAN",12,1);
		OLED_ShowString(0,48,"2014/05/01",16,1);
		OLED_ShowString(0,64,"ASCII:",16,1);  
		OLED_ShowString(0,80,"CODE:",16,1);
		OLED_ShowChar(48,64,t,16,1);//��ʾASCII�ַ�	   
		t++;
		if(t>'~')t=' ';
		OLED_ShowNum(40,80,t,3,16,1);
		OLED_Refresh();
		delay_ms(500);
		OLED_Clear();
		OLED_ShowChinese(0,8,0,16,1);  //16*16 ��
	  OLED_ShowChinese(16,4,0,24,1); //24*24 ��
		OLED_ShowChinese(40,0,0,32,1);//32*32 ��
	  OLED_ShowChinese(0,32,0,64,1); //64*64 ��
		OLED_ShowString(0,110,"AB",8,1);//6*8 ��ABC��
		OLED_ShowString(14,108,"CD",12,1);//6*12 ��ABC��
	  OLED_ShowString(28,106,"EF",16,1);//8*16 ��ABC��
		OLED_ShowString(46,102,"GH",24,1);//12*24 ��ABC��
	  OLED_Refresh();
		delay_ms(500);
		OLED_ScrollDisplay(11,4,1);
	}
}

