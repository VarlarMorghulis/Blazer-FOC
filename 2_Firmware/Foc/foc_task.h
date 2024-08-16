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
	/*����״̬��־λ*/
	uint8_t Step;
	/*�жϼ�������*/
	uint32_t Cnt_20kHz;
	/*����״̬*/
	ErrorState Errstate;
}TaskElement_TypeDef;

void FOC20kHzIRQHandler(void);
void FOC_ErrorHandle(void);

#endif