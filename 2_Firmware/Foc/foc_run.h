#ifndef __FOC_RUN_H__
#define __FOC_RUN_H__

#include "main.h"

typedef enum
{
	FOC_Current_Mode,
	FOC_CurrentSpeed_Mode,
}FOC_Sensored_Mode; 

typedef struct
{
	uint8_t Pole_Pairs;
	float Rs;
	float Ls;
	float Flux;
}Motor_TypeDef;

#include "common_inc.h"

void Open_Voltageloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs);
void Sensored_Currentloop(void);
void Sensored_Speedloop(void);
void FOC_Sensored_Positionloop(void);
void FOC_Task_Sensored(void);
void FOC_Task_Sensorless(void);
void FOC_Task_Openloop(void);
#endif