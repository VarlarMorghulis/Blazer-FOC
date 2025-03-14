#include "foc_run.h"

PID_TypeDef PID_Speed;

PID_TypeDef PID_ZeroSpeed;

PID_TypeDef PID_Position=
{
	.Kp=0.1f,
	.output_max=100.0f
};

extern MotorControl_TypeDef MotorControl;
extern Encoder_TypeDef Encoder;

uint8_t speed_to_pos;
uint8_t spdloop_state = Speed_Mode;
uint8_t spdloop_laststate = Speed_Mode;

void Speedloop_StateReset(void)
{
	speed_to_pos = 0;
	spdloop_state = Speed_Mode;
	spdloop_laststate = Speed_Mode;
}

/**
   * @brief  有感模式速度环函数 10kHz频率
   * @param  无
   * @retval 无
   */
void Sensored_Speedloop(void)
{
//	static uint8_t position_cnt;
//	
//	/*启用位置模式,零速转矩保持*/
//	if(PID_Speed.ref_value==0)
//	{	
//		if(fast_abs(Encoder.velocity)<= _2PI / 10.0f)
//		{
//			spdloop_state=Position_Mode;
//			
//			Clear_PID_Param(&PID_Speed);
//			if(spdloop_laststate==Speed_Mode)
//			{
//				PID_Position.ref_value=Encoder.sensor_dir * SPI_Encoder_GetCalAngle(&Encoder);
//				speed_to_pos=1;
//			}
//		}
//		
//		if(speed_to_pos==1)
//		{
//			if(++position_cnt>=2)
//			{	
//				PID_Position.samp_value = Encoder.sensor_dir * SPI_Encoder_GetCalAngle(&Encoder);
//				PID_ZeroSpeed.ref_value=Position_P_Ctrl(&PID_Position);
//				position_cnt=0;
//			}
//			PID_ZeroSpeed.samp_value=Encoder.velocity;
//			PID_Iq.ref_value=Speed_PI_Ctrl(&PID_ZeroSpeed);
//		}
//		else if(speed_to_pos==0)
//		{
//			PID_Speed.samp_value=Encoder.velocity;
//			PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
//		}
//	}
//	else
//	{
//		spdloop_state=Speed_Mode;
//		speed_to_pos=0;
//		
//		Clear_PID_Param(&PID_ZeroSpeed);
//		
//		PID_Speed.samp_value=Encoder.velocity;
//		PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
//	}
//	
//	spdloop_laststate=spdloop_state;
	
	if(MotorControl.speedAcc == 0.0f || MotorControl.speedDec == 0.0f)
		MotorControl.isUseSpeedRamp = 0;
	else
		MotorControl.isUseSpeedRamp = 1;
	
	/*启用速度爬升*/
	if(MotorControl.isUseSpeedRamp == 1)
	{
		if(MotorControl.speedRef > MotorControl.speedShadow)
		{
			MotorControl.speedShadow += MotorControl.speedAcc * _2PI * Speed_Ts;
			if(MotorControl.speedShadow > MotorControl.speedRef)
				MotorControl.speedShadow = MotorControl.speedRef;
		}
		else if(MotorControl.speedRef < MotorControl.speedShadow)
		{
			MotorControl.speedShadow -= MotorControl.speedDec * _2PI * Speed_Ts;
			if(MotorControl.speedShadow < MotorControl.speedRef)
				MotorControl.speedShadow = MotorControl.speedRef;			
		}
	}
	else
	{
		MotorControl.speedShadow = MotorControl.speedRef;
	}
	
	PID_Speed.Kp = MotorControl.speed_Kp;
	PID_Speed.Ki = MotorControl.speed_Ki;
	PID_Speed.samp_value = Encoder_GetMecVel();
	PID_Speed.ref_value  = MotorControl.speedShadow;
	MotorControl.iqRef   = Speed_PI_Ctrl(&PID_Speed) * MotorControl.current_limit;
}

void Sensored_Positionloop(void)
{

}

void Task_Current_Mode(void)
{	
	float phase;
	float phase_vel;
	
	if(MotorControl.isUseSensorless == true)
	{
		phase = Observer_GetElePhase();
		phase_vel = 0.0f;
	}
	else
	{
		phase = Encoder_GetElePhase();
		phase_vel = Encoder_GetEleVel();
	}   

	FOC_Current(MotorControl.idRef, MotorControl.iqRef, phase, phase_vel);
}

void Task_Speed_Mode(void)
{
	static int speedloop_count;
	
	float phase;
	float phase_vel;
	
	if(MotorControl.isUseSensorless == true)
	{
		phase = Observer_GetElePhase();
		phase_vel = Observer_GetEleVel();
	}
	else
	{
		phase = Encoder_GetElePhase();
		phase_vel = Encoder_GetEleVel();
	}   
	
	if(++speedloop_count >= 2)
	{
		if(MotorControl.speedAcc == 0.0f || MotorControl.speedDec == 0.0f)
			MotorControl.isUseSpeedRamp = 0;
		else
			MotorControl.isUseSpeedRamp = 1;
		
		/*启用速度爬升*/
		if(MotorControl.isUseSpeedRamp == 1)
		{
			if(MotorControl.speedRef > MotorControl.speedShadow)
			{
				MotorControl.speedShadow += MotorControl.speedAcc * _2PI * Speed_Ts;
				if(MotorControl.speedShadow > MotorControl.speedRef)
					MotorControl.speedShadow = MotorControl.speedRef;
			}
			else if(MotorControl.speedRef < MotorControl.speedShadow)
			{
					MotorControl.speedShadow -= MotorControl.speedDec * _2PI * Speed_Ts;
					if(MotorControl.speedShadow < MotorControl.speedRef)
						MotorControl.speedShadow = MotorControl.speedRef;			
			}
		}
		else
		{
			MotorControl.speedShadow = MotorControl.speedRef;
		}
		PID_Speed.Kp = MotorControl.speed_Kp;
		PID_Speed.Ki = MotorControl.speed_Ki;
		PID_Speed.samp_value = phase_vel / (float)MotorControl.motor_pole_pairs;
		PID_Speed.ref_value  = MotorControl.speedShadow;
		MotorControl.iqRef   = Speed_PI_Ctrl(&PID_Speed) * MotorControl.current_limit;
		
		speedloop_count = 0;
	}
	
	FOC_Current(MotorControl.idRef, MotorControl.iqRef, phase, phase_vel);
}

void Task_Position_Mode(void)
{
	
}