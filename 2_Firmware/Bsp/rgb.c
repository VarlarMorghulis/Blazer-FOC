#include "rgb.h"

/*��ά����DMAҪ���������*/
uint32_t Pixel_Buf[LED_NUM+1][24];

RGB_Color RGB;
uint8_t rgb_up_down = 0;
int brightness = 0;


/*
���ܣ��趨����RGB LED����ɫ���ѽṹ����RGB��24BITת��Ϊ0���1��
������LedIdΪLED��ţ�Color���������ɫ�ṹ��
*/
void RGB_SetColor(uint8_t LedId, RGB_Color Color)
{
    uint8_t i;
    if(LedId > LED_NUM){
        return; //avoid overflow ��ֹд��ID����LED����
    }

    for(i=0; i<8; i++) {
        Pixel_Buf[LedId][i] = ( (255-Color.G & (1 << (7 - i))) ? (CODE_1) : CODE_0);//����ĳһ��0~7ת�����G
    }
    for(i=8; i<16; i++)  {
        Pixel_Buf[LedId][i] = ( (255-Color.R & (1 << (15 - i))) ? (CODE_1) : CODE_0);//����ĳһ��8~15ת�����R
    }
    for(i=16; i<24; i++) {
        Pixel_Buf[LedId][i] = ( (255-Color.B & (1 << (23 - i))) ? (CODE_1) : CODE_0);//����ĳһ��16~23ת�����B
    }
}

/*
���ܣ����һ��װ��24��0�����24������ռ�ձ�Ϊ0��PWM������Ϊ���reset��ʱ��������ʱ��Ϊ24*1.2=30us > 24us(Ҫ�����24us)
*/
void Reset_Load(void)
{
    uint8_t i;
    for(i=0; i<24; i++)
    {
        Pixel_Buf[LED_NUM][i] = 0;
    }
}

/*
���ܣ���������
*/
void RGB_SendArray(void)
{
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf, (LED_NUM+1)*24);
}

/*
д����ɫ
*/
void write_color(uint16_t Pixel_Len,RGB_Color color)
{
    uint16_t i;
    for(i=0; i<Pixel_Len; i++)//����Ӧ����LEDд����ɫ
    {
        RGB_SetColor(i,color);
    }
    Reset_Load();
    RGB_SendArray();
}

void Set_RGB_BreathingColor(COLOR color)
{
	if(rgb_up_down == 0)
	{
		brightness += 4;
		if(brightness >= 100)
		{
			brightness = 100;
			rgb_up_down = 1;
		}
	}
	else if(rgb_up_down == 1)
	{
		brightness -= 4;
		if(brightness <= -40)
		{
			brightness = -40;
			rgb_up_down = 0;
		}
	}
	
	switch(color)
	{
		case COLOR_NULL:
			RGB.R = 0;
			RGB.G = 0;
			RGB.B = 0;
		break;
		
		case RED:
			RGB.R = brightness >= 0 ? brightness : 0;
			RGB.G = 0;
			RGB.B = 0;	
		break;
		
		case GREEN:
			RGB.R = 0;
			RGB.G = brightness >= 0 ? brightness : 0;
			RGB.B = 0;
		break;
		
		case BLUE:
			RGB.R = 0;
			RGB.G = 0;
			RGB.B = brightness >= 0 ? brightness : 0;
		break;
		
		case YELLOW:
			RGB.R = brightness >= 0 ? brightness : 0;
			RGB.G = brightness >= 0 ? brightness : 0;
			RGB.B = 0;
		break;
		
		case PURPLE:
			RGB.R = brightness >= 0 ? brightness : 0;
			RGB.G = 0;
			RGB.B = brightness >= 0 ? brightness : 0;
		break;
		
		case CYAN:
			RGB.R = 0;
			RGB.G = brightness >= 0 ? brightness : 0;
			RGB.B = brightness >= 0 ? brightness : 0;
		break;
		
		case WHITE:
			RGB.R = brightness >= 0 ? brightness : 0;
			RGB.G = brightness >= 0 ? brightness : 0;
			RGB.B = brightness >= 0 ? brightness : 0;
		break;
		
		default:break;
	}
	
	write_color(1, RGB);
}