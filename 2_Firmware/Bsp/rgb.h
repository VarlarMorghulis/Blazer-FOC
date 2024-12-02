#ifndef __RGB_H__
#define __RGB_H__

#include "main.h"
#include "tim.h"

/*定时器生成WS2812码型的CCR*/
#define CODE_1		(29)       //生成1码定时器计数次数
#define CODE_0		(59)      //生成0码定时器计数次数

/*总线上的LED个数*/
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

/*定义单个LED三原色值大小*/
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