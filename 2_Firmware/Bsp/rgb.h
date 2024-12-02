#ifndef __RGB_H__
#define __RGB_H__

#include "main.h"
#include "tim.h"

/*��ʱ������WS2812���͵�CCR*/
#define CODE_1		(29)       //����1�붨ʱ����������
#define CODE_0		(59)      //����0�붨ʱ����������

/*�����ϵ�LED����*/
#define LED_NUM		1

typedef enum
{
	COLOR_NULL,
	RED,
	GREEN,
	BLUE,
	YELLOW,
	PURPLE,
	CYAN,
	WHITE,
}COLOR;

/*���嵥��LED��ԭɫֵ��С*/
typedef struct
{
	uint8_t R;
	uint8_t G;
	uint8_t B;
}RGB_Color;

void RGB_SetColor(uint8_t LedId, RGB_Color Color);
void Reset_Load(void);
void RGB_SendArray(void);
void write_color(uint16_t Pixel_Len,RGB_Color color);
void Set_RGB_BreathingColor(uint8_t color);

#endif