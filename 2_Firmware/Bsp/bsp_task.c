#include "bsp_task.h"

uint16_t Led_Cnt;
uint16_t Analog_Cnt;
uint16_t Key_Cnt;
uint16_t Menu_Cnt;
uint8_t menu_key;

void BSP1kHzIRQHandler(void)
{
	Vofa_Upload();
	
	if(++ Led_Cnt >= 200)
	{
		LED_Task();
		Led_Cnt=0;
	}
		
	if(++ Analog_Cnt >= 1)
	{
		Temperature_Update();
		Analog_Cnt = 0;
	}
	
	if(++ Key_Cnt >= 50)
	{
		Key_Status_Check();
		menu_key = Menu_GetKeyval();
		Key_Cnt = 0;
	}
	
	Menu_Cnt ++;
	
	CAN_DisConnect_Handle();
}