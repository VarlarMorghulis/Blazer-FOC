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

typedef struct
{
	/*运行状态标志位*/
	uint8_t Step;
	/*中断计数变量*/
	uint32_t Cnt_20kHz;
	/*运行状态*/
	ErrorState Errstate;
}TaskElement_TypeDef;

void FOC20kHzIRQHandler(void);
void FOC_ErrorHandle(void);

#endif