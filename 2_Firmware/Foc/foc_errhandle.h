#ifndef __FOC_ERRHANDLE_H__
#define __FOC_ERRHANDLE_H__

#include "main.h"
#include "foc_algorithm.h"
#include "foc_pid.h"
#include "data_type.h"

void Set_ErrorNow(ErrorNow_TypeDef tErrorNow);
void Clear_RunningData(void);
void Stop_PWM_Generate(void);
void Stop_PWM_Generate(void);
void Start_PWM_Generate(void);

#endif