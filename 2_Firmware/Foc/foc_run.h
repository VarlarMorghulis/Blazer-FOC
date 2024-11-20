#ifndef __FOC_RUN_H__
#define __FOC_RUN_H__

#include "common_inc.h"

void Speedloop_StateReset(void);
void Sensored_Speedloop(void);
void Sensored_Positionloop(void);

void Task_Current_Mode(void);
void Task_Speed_Mode(void);

#endif