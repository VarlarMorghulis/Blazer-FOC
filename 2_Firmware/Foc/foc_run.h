#ifndef __FOC_RUN_H__
#define __FOC_RUN_H__

#include "main.h"
#include <stdbool.h>

typedef enum
{
	Current_Mode=0,
	SpeedCurrent_Mode,
	PositionSpeedCurrent_Mode,
}Control_Mode_TypeDef; 

typedef struct
{
	uint8_t Pole_Pairs;
	float Rs;
	float Ls;
	float Flux;
}Motor_TypeDef;

typedef struct
{
	Control_Mode_TypeDef Control_Mode;
	
	bool isUseSpeedRamp;
	bool isUseZeroSpeedTorqueMaintain;
	float speedRef;
	float speedShadow;
	float speedAcc;
	float speedDec;
}MotorControl_TypeDef;

#include "common_inc.h"

void Open_Voltageloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs);
void Open_Currentloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs,float runtime);
void FOC_Task_Openloop(void);

void Sensored_Currentloop(void);
void Speedloop_StateReset(void);
void Sensored_Speedloop(void);
void Sensored_Positionloop(void);
void FOC_Task_Sensored(void);

void Sensorless_Currentloop(void);
void FOC_Task_Sensorless(void);

#endif