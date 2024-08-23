#include "bsp_task.h"

uint16_t Led_Cnt;
uint16_t Analog_Cnt;
uint16_t Key_Cnt;
uint16_t Menu_Cnt;
uint16_t CanTx_Cnt;
uint8_t menu_key;
uint32_t CAN_Rx_timeout;
uint16_t MT6816_Raw;
extern uint8_t Z_confirm_flag;
extern uint8_t CAN_Txflag;
extern FOC_State FOC_State_t;

void BSP1kHzIRQHandler(void)
{
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
	
	if(FOC_State_t==FOC_Sensored)
	{
		/*CAN_Rx_timeout每次在CANRxIRQHandler()中清零*/
		if(++CAN_Rx_timeout>=500)
		{
			CAN_LostConnect_Handle();
		}
	}
}