#include "bsp_task.h"

uint16_t Led_Cnt;
uint16_t Analog_Cnt;
uint16_t Key_Cnt;
uint16_t Menu_Cnt;
uint8_t menu_key;
uint32_t CAN_Rx_timeout;

void BSP1kHzIRQHandler(void)
{
	//CAN_SendMessage();
	Vofa_Upload();
	
	if(++Led_Cnt>=500)
	{
		FOC_ErrorHandle();
		Led_Cnt=0;
	}
	
	if(++Analog_Cnt>=1)
	{
		Temperature_Update();
		Vbus_Update();
		Ibus_Update();
		Analog_Cnt=0;
	}
	
	if(++Key_Cnt>=50)
	{
		Key_Status_Check();
		menu_key=Menu_GetKeyval();
		Key_Cnt=0;
	}
	
	Menu_Cnt++;
	
	/*CAN_Rx_timeout每次在CANRxIRQHandler()中清零*/
	if(++CAN_Rx_timeout>=500)
	{
		//CAN_LostConnect_Handle();
	}
}