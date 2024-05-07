#include "foc_task.h"

FOC_State FOC_State_t=FOC_Reminder;

/**
   * @brief  FOC一级状态机
   * @param  无
   * @retval 无
   */
void FOC20kHzIRQHandler(void)
{
	switch(FOC_State_t)
	{
		/*上电蜂鸣提示*/
		case FOC_Reminder:
		{
			FOC_Task_Reminder();
		}
		break;
		
		/*校准任务*/
		case FOC_Calibration:
		{
			FOC_Task_Calibration();
		}
		break;
				
		case FOC_Sensored:
		{
			FOC_Task_Sensored();
		}
		break;
		
		case FOC_Sensorless:
		{
			FOC_Task_Sensorless();
		}
		break;
		
		case FOC_Wait:
		{
			Motor_Release();
		}
		break;
		
		case FOC_Error:
		{
			Motor_Release();
		}
		
		default:break;
	}
	
}

extern TaskElement_TypeDef TE_Currentloop_t;
extern TaskElement_TypeDef TE_Speedloop_t;
extern TaskElement_TypeDef TE_ADC_Calibration_t;
extern TaskElement_TypeDef TE_Encoder_Calibration_t;
extern AnalogParam_TypeDef AnalogParam_t;
void FOC_ErrorHandle(void)
{
	if(TE_Currentloop_t.Errstate==FOC_FAULT||
	   TE_Speedloop_t.Errstate==FOC_FAULT||
	   TE_ADC_Calibration_t.Errstate==FOC_FAULT||
	   TE_Encoder_Calibration_t.Errstate==FOC_FAULT)
	{
		LED_Error();
	}
	else if(FOC_State_t==FOC_Calibration)
	{
		LED_Calib();
	}
	else
	{
		LED_Normal();
	}
}