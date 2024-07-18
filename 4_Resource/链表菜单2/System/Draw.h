#ifndef __DRAW_H
#define __DRAW_H

extern uint8_t OLED_GRAM[8][128];

void Draw_Clear(void);
void Draw_Refresh(void);
void Draw_Refresh_Set(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);
void Draw_Point(uint8_t x, uint8_t y);
void Draw_Line(int8_t x0, int8_t y0, int8_t x1, int8_t y1);
void Draw_Circle(uint8_t x, uint8_t y, uint8_t r);
void Draw_Circle_Solid(uint8_t x, uint8_t y, uint8_t r, uint8_t width);
void Draw_Circal_Box(uint8_t x, uint8_t y, uint8_t r, uint8_t length);
void Draw_Circal_Box_Solid(uint8_t x, uint8_t y, uint8_t r, uint8_t length, uint8_t width);
void Draw_Slider(uint8_t x, uint8_t y, uint8_t length, uint8_t position);
void Draw_Triangle(int8_t x0, int8_t y0, int8_t x1, int8_t y1, int8_t x2, int8_t y2);

#endif

