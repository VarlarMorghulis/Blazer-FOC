#ifndef __FOC_TASK_H__
#define __FOC_TASK_H__

#include "common_inc.h"

typedef enum
{
	FOC_Reminder,
	FOC_Calibration,
	FOC_Identification,
	FOC_Sensored,
	FOC_Sensorless,
	FOC_Error,
	FOC_Wait
}FOC_State;

typedef struct
{
	/*初始化标志位 0执行初始化 1不执行*/
	uint8_t Init_Flag;
	/*运行任务标志位 0运行 1不运行*/
	uint8_t Run_Flag;
	/*中断计数变量*/
	uint32_t Cnt_20kHz;
	/*运行状态*/
	ErrorState Errstate;
}TaskElement_TypeDef;

void FOC20kHzIRQHandler(void);
void FOC_ErrorHandle(void);

#endif