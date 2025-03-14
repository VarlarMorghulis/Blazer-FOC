#include "bsp_task.h"

uint16_t Led_Cnt;
uint16_t RGB_Cnt;
uint16_t Analog_Cnt;
uint16_t Key_Cnt;
uint16_t Menu_Cnt;
uint8_t menu_key;

extern MotorControl_TypeDef MotorControl;

void BSP1kHzIRQHandler(void)
{
	Vofa_Upload();
	
	if(++Led_Cnt >= 200)
	{
		LED_Task();
		Led_Cnt=0;
	}
		
	if(++RGB_Cnt >= 50)
	{
		switch(MotorControl.ModeNow)
		{
			case Current_Mode:
				Set_RGB_BreathingColor(GREEN);
			break;
			
			case Speed_Mode:
				Set_RGB_BreathingColor(CYAN);
			break;
			
			case Position_Mode:
				Set_RGB_BreathingColor(BLUE);
			break;
			
			case Calib_Motor_R_L_Flux:
				Set_RGB_BreathingColor(PURPLE);
			break;
			
			case Calib_EncoderOffset:
				Set_RGB_BreathingColor(PURPLE);
			break;
			
			default:
				Set_RGB_BreathingColor(COLOR_NULL);
			break;
		}
		
		RGB_Cnt = 0;
	}
	
	if(++Analog_Cnt >= 1)
	{
		Temperature_Update();
		Analog_Cnt = 0;
	}
	
	if(++Key_Cnt >= 50)
	{
		Key_Status_Check();
		menu_key = Menu_GetKeyval();
		Key_Cnt = 0;
	}
	
	Menu_Cnt ++;
	
	CAN_DisConnect_Handle();
}