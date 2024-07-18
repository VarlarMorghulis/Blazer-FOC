#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "menu.h"
#include "ADC.h"
#include "Draw.h"
#include "menu.h"

uint8_t i,j;

int main(void)
{
	OLED_Init();
	ACD_Init();
	Key_Init();
	Menu_Init();
	
	OLED_Clear();
	
	while (1)
	{	
		Choose_Menu();
	}
}
