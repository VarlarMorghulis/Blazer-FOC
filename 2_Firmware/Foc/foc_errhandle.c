#include "foc_errhandle.h"

extern MotorControl_TypeDef MotorControl;
extern FOC_TypeDef FOC;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_Position;
void Set_ErrorNow(ErrorNow_TypeDef tErrorNow)
{
	MotorControl.ErrorNow = tErrorNow;
}

void Clear_RunningData(void)
{
	MotorControl.idRef		 = 0.0f;
	MotorControl.iqRef		 = 0.0f;
	MotorControl.speedRef    = 0.0f;
	MotorControl.speedShadow = 0.0f;
	
	FOC.Id     = 0.0f;
	FOC.Iq     = 0.0f;
	FOC.Vd_int = 0.0f;
	FOC.Vq_int = 0.0f;
	
	PID_Speed.error_sum    = 0.0f;
	PID_Position.error_sum = 0.0f;
}

