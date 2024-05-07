#ifndef __OLED_H_
#define __OLED_H_	

#define	OLED_ADRESS	0x78
#include "stm32f10x.h"                  // Device header


void OLED_Write_CMD(uint8_t Byte);
void OLED_Write_Data(uint8_t Byte);
void OLED_Init(void);
void OLED_Fill(uint8_t Fill_Data);
void OLED_Clear(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Refresh(void);
void OLED_SetPos(uint8_t x,uint8_t y);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowChar2(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowChar_F6x8(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char* String);
void OLED_ShowString2(uint8_t Line, uint8_t Column, char* String);
void OLED_ShowString_F6x8(uint8_t Line, uint8_t Column, char* String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowNum_F6x8(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum_F6x8(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t num);
void OLED_ShowChinese2(uint8_t Line,uint8_t Column,uint8_t num);

#endif
