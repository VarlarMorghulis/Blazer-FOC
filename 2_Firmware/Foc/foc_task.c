#include "foc_task.h"

MotorControl_TypeDef MotorControl;

ModeNow_TypeDef  ModeLast  = Motor_Disable;
ErrorNow_TypeDef ErrorLast = No_Error;

void MotorControl_Init(void)
{
	MotorControl.ModeNow  = Motor_Disable;
	MotorControl.ErrorNow = No_Error;
	
	MotorControl.isUseSpeedRamp = 0;
	MotorControl.isUseZeroSpeedTorqueMaintain = 0;
	
	MotorControl.id_Kp = 0.00153;
	MotorControl.id_Ki = 6.34;
	MotorControl.iq_Kp = 0.00153;
	MotorControl.iq_Ki = 6.34;
	
	MotorControl.speed_Kp = 0.05f;
	MotorControl.speed_Ki = 0.5f;
	
	MotorControl.pos_Kp = 0.1f;
	MotorControl.pos_Ki = 0.0f;
	
	MotorControl.calib_current = 7.0f;
	//MotorControl.current_limit = 30.0f;
	MotorControl.speed_limit   = 100.0f;
}

/**
   * @brief  FOC一级状态机
   * @param  无
   * @retval 无
   */
void FOC20kHzIRQHandler(void)
{
	Vbus_Update();
	
	Current_Cal();
	
	if(MotorControl.isUseSensorless == true)
	{
		Fluxobserver_Update();
	}
	else
	{
		Encoder_Update();
	}
	
	switch(MotorControl.ModeNow)
	{
		case Motor_Disable:
			//PWM_TurnOnHighSides();
		break;
		
		case Current_Mode:
			Task_Current_Mode();
		break;
		
		case Speed_Mode:
			Task_Speed_Mode();
		break;
		
		case Position_Mode:
			
		break;
		
		case Calib_Motor_R_L_Flux:
			Task_Calib_R_L_Flux();
		break;
		
		case Calib_EncoderOffset:
			Task_Calib_Encoder();
		break;
		
		case Default_Param:
			Param_Return_Default();
		
		case Clear_Error:
			Set_ErrorNow(No_Error);
		default:break;
	}
	
	
	/*无错误*/
	if(MotorControl.ErrorNow == No_Error)
	{
		LED_SetState(0, (uint8_t)MotorControl.ModeNow);
	}
	/*有错误*/
	else
	{
		/*存储参数时可容忍错误*/
		if(MotorControl.ModeNow != Save_Param && MotorControl.ModeNow != Default_Param)
			MotorControl.ModeNow = Motor_Disable;
		
		LED_SetState(1, (uint8_t)MotorControl.ErrorNow);
	}
	
	if(ModeLast != Motor_Disable && MotorControl.ModeNow == Motor_Disable)
	{
		Clear_RunningData();
	}
	
	if(ErrorLast == No_Error && MotorControl.ErrorNow != No_Error)
	{
		Stop_PWM_Generate();
	}
	
	if(ErrorLast != No_Error && MotorControl.ErrorNow == No_Error)
	{
		Start_PWM_Generate();
	}
	
	ModeLast  = MotorControl.ModeNow;
	ErrorLast = MotorControl.ErrorNow;
}
