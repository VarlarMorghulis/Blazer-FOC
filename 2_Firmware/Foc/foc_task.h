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
	/*��ʼ����־λ 0ִ�г�ʼ�� 1��ִ��*/
	uint8_t Init_Flag;
	/*���������־λ 0���� 1������*/
	uint8_t Run_Flag;
	/*�жϼ�������*/
	uint32_t Cnt_20kHz;
	/*����״̬*/
	ErrorState Errstate;
}TaskElement_TypeDef;

void FOC20kHzIRQHandler(void);
void FOC_ErrorHandle(void);

#endif