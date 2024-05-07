#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"
#include "ADC.h"

enum {NoKEY, ENTER, UP, DOWN, LEFT, RIGHT};

void Key_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = NoKEY;
	if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == RESET)
	{
		Delay_ms(TIME);
		while (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4) == RESET);
		Delay_ms(TIME);
		KeyNum = ENTER;
	}
	if (ADC_Getvalue(ADC_Channel_0) >= 3500)
	{
		Delay_ms(TIME);
		while (ADC_Getvalue(ADC_Channel_0) >= 3500);
		Delay_ms(TIME);
		KeyNum = DOWN;
	}
	if (ADC_Getvalue(ADC_Channel_0) <= 500)
	{
		Delay_ms(TIME);
		while (ADC_Getvalue(ADC_Channel_0) <= 500);
		Delay_ms(TIME);
		KeyNum = UP;
	}
	if (ADC_Getvalue(ADC_Channel_1) >= 3500)
	{
		Delay_ms(TIME);
		while (ADC_Getvalue(ADC_Channel_1) >= 3500);
		Delay_ms(TIME);
		KeyNum = LEFT;
	}
	if (ADC_Getvalue(ADC_Channel_1) <= 500)
	{
		Delay_ms(TIME);
		while (ADC_Getvalue(ADC_Channel_1) <= 500);
		Delay_ms(TIME);
		KeyNum = RIGHT;
	}
	return KeyNum;
}
