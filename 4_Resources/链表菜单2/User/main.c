#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "menu.h"
#include "ADC.h"
#include "Draw.h"
#include "menu.h"
#include "Draw.h"
#include "Draw_Font.h"


int main(void)
{
	OLED_Init();
	ACD_Init();
	Key_Init();
	Menu_Init();	
	OLED_Clear();
	Menu_Show(0);
	while (1)
	{	
		Choose_Menu();
	}
}
