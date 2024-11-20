#ifndef __FOC_TASK_H__
#define __FOC_TASK_H__

#include "common_inc.h"

typedef enum
{
	FOC_Reminder,
	FOC_Calibration,
	FOC_Identification,
	FOC_Openloop,
	FOC_Sensored,
	FOC_Sensorless,
	FOC_Error,
	FOC_Wait
}FOC_State;

void MotorControl_Init(void);
void FOC20kHzIRQHandler(void);

#endif