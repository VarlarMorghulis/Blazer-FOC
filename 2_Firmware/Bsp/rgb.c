#include "rgb.h"

/*二维数组DMA要传输的数据*/
uint32_t Pixel_Buf[LED_NUM+1][24];

RGB_Color RGB;
uint8_t rgb_up_down = 0;
int brightness = 0;


/*
功能：设定单个RGB LED的颜色，把结构体中RGB的24BIT转换为0码和1码
参数：LedId为LED序号，Color：定义的颜色结构体
*/
void RGB_SetColor(uint8_t LedId, RGB_Color Color)
{
    uint8_t i;
    if(LedId > LED_NUM){
        return; //avoid overflow 防止写入ID大于LED总数
    }

    for(i=0; i<8; i++) {
        Pixel_Buf[LedId][i] = ( (255-Color.G & (1 << (7 - i))) ? (CODE_1) : CODE_0);//数组某一行0~7转化存放G
    }
    for(i=8; i<16; i++)  {
        Pixel_Buf[LedId][i] = ( (255-Color.R & (1 << (15 - i))) ? (CODE_1) : CODE_0);//数组某一行8~15转化存放R
    }
    for(i=16; i<24; i++) {
        Pixel_Buf[LedId][i] = ( (255-Color.B & (1 << (23 - i))) ? (CODE_1) : CODE_0);//数组某一行16~23转化存放B
    }
}

/*
功能：最后一行装在24个0，输出24个周期占空比为0的PWM波，作为最后reset延时，这里总时长为24*1.2=30us > 24us(要求大于24us)
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
功能：发送数组
*/
void RGB_SendArray(void)
{
    HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)Pixel_Buf, (LED_NUM+1)*24);
}

/*
写入颜色
*/
void write_color(uint16_t Pixel_Len,RGB_Color color)
{
    uint16_t i;
    for(i=0; i<Pixel_Len; i++)//给对应个数LED写入颜色
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