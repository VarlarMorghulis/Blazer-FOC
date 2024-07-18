#include "stm32f10x.h"                  // Device header
#include "OLED.h"
#include "my_SPI.h"

uint8_t OLED_GRAM[8][128];

void Draw_Refresh(void)
{
	uint8_t i, j;
	OLED_W_CS(0);
	for(i=0;i<8;i++)
	{
		OLED_W_DC(0);
		OLED_SPI_SendByte(0xb0+i);
		OLED_SPI_SendByte(0x00);
		OLED_SPI_SendByte(0x10);
		OLED_W_DC(1);
		for(j=0;j<128;j++)
		{		
			OLED_SPI_SendByte(OLED_GRAM[i][j]);
			
		}
	}
	OLED_W_CS(1);
}

void Draw_Refresh_Set(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
	uint8_t i, j;
	OLED_W_CS(0);
	for(i=y0;i<=y1;i++)
	{
		OLED_W_DC(0);
		OLED_SPI_SendByte(0xb0+i);
		OLED_SPI_SendByte(0x00);
		OLED_SPI_SendByte(0x10);
		OLED_W_DC(1);
		for(j=x0;j<=x1;j++)
		{		
			OLED_SPI_SendByte(OLED_GRAM[i][j]);
			
		}
	}
	OLED_W_CS(1);
}

void Draw_Clear(void)
{
    uint8_t i, j;
    for(i=0;i<8;i++)
    {
        for(j=0;j<128;j++)
        {
            OLED_GRAM[i][j]=0x00;
        }
    }
}

void Draw_Point(uint8_t x, uint8_t y)
{
    uint8_t temp = 0;
    if(x > 127 || y > 63)
        return;
    temp = 1 << (y % 8);
    OLED_GRAM[y / 8][x] |= temp;
}

void Draw_Line(int8_t x0, int8_t y0, int8_t x1, int8_t y1)
{
	uint8_t x_long = (x1 - x0 >= 0) ? (x1 - x0) : (x0 - x1);
	uint8_t y_long = (y1 - y0 >= 0) ? (y1 - y0) : (y0 - y1);
	uint8_t i;
	float max = (x_long >= y_long) ? x_long : y_long;
	
	float dx = (float)(x1 - x0) / max;
	float dy = (float)(y1 - y0) / max;
	
	for(i=0;i<max;i++)
	{
		Draw_Point(x0+dx*i+0.5, y0+dy*i+0.5);
	}
}

void Draw_Circle(uint8_t x, uint8_t y, uint8_t r)
{
	uint8_t a = 0;
    uint8_t b = r;
    while (a <= b)
    {
        Draw_Point(x - b, y - a);
        Draw_Point(x - b, y + a);
        Draw_Point(x - a, y + b);
        Draw_Point(x - a, y - b);
		
        Draw_Point(x + b, y - a);
        Draw_Point(x + b, y + a);
        Draw_Point(x + a, y + b);
        Draw_Point(x + a, y - b);
        a++;
        if ((a * a + b * b) > (r * r))
        {
            b--;
        }
    }
}

void Draw_Circle_Solid(uint8_t x, uint8_t y, uint8_t r, uint8_t width)
{
	uint8_t a, b;
	while(width--)
	{
		a = 0;
		b = r;
		while (a <= b)
		{
			Draw_Point(x - b, y - a);
			Draw_Point(x - b, y + a);
			Draw_Point(x - a, y + b);
			Draw_Point(x - a, y - b);
			
			Draw_Point(x + b, y - a);
			Draw_Point(x + b, y + a);
			Draw_Point(x + a, y + b);
			Draw_Point(x + a, y - b);
			a++;
			if ((a * a + b * b) > (r * r))
			{
				b--;
			}
		}
		r--;
	}
}

void Draw_Circal_Box(uint8_t x, uint8_t y, uint8_t r, uint8_t length)
{
	uint8_t a = 0;
    uint8_t b = r;
    while (a <= b)
    {
        Draw_Point(x - b - length, y - a);
        Draw_Point(x - b - length, y + a);
        Draw_Point(x - a - length, y + b);
        Draw_Point(x - a - length, y - b);
		
        Draw_Point(x + b + length, y - a);
        Draw_Point(x + b + length, y + a);
        Draw_Point(x + a + length, y + b);
        Draw_Point(x + a + length, y - b);
        a++;
        if ((a * a + b * b) > (r * r))
        {
            b--;
        }
    }
	Draw_Line(x - length, y - r, x + length, y - r);
    Draw_Line(x - length, y + r, x + length, y + r);
}

void Draw_Circal_Box_Solid(uint8_t x, uint8_t y, uint8_t r, uint8_t length, uint8_t width)
{
	uint8_t a, b;
	while(width--)
	{
		a = 0;
		b = r;
		while (a <= b)
		{
			Draw_Point(x - b - length, y - a);
			Draw_Point(x - b - length, y + a);
			Draw_Point(x - a - length, y + b);
			Draw_Point(x - a - length, y - b);
			
			Draw_Point(x + b + length, y - a);
			Draw_Point(x + b + length, y + a);
			Draw_Point(x + a + length, y + b);
			Draw_Point(x + a + length, y - b);
			a++;
			if ((a * a + b * b) > (r * r))
			{
				b--;
			}
		}
		Draw_Line(x - length, y - r, x + length, y - r);
		Draw_Line(x - length, y + r, x + length, y + r);
		r--;
	}
}

void Draw_Slider(uint8_t x, uint8_t y, uint8_t length, uint8_t position)
{
    uint8_t i;
    Draw_Circal_Box_Solid(x, y, 2, length, 1);
    Draw_Circle_Solid(x+position-length, y, 3, 3);
    for(i=0;i<3;i++)
        Draw_Line(x-length, y-1+i, x+position-length, y-1+i);
}

void Draw_Triangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2)
{
	uint16_t i;
	for(i=0;i<8;i++)
	{
		Draw_Line(x1,y1,x2,y2);
		if(x0>x1 && x0>x2)
			{x1--;x2--;y1++;y2--;}
		else if(x0<x1 && x0<x2)
			{x1++;x2++;y1++;y2--;}
		else if(y0>y1 && y0>y2)
			{x1++;x2--;y1++;y2++;}
		else if(y0<y1 && y0<y2)
			{x1++;x2--;y1--;y2--;}
	}
	
}
