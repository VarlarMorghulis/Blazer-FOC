#ifndef __FOC_PARAM_H__
#define __FOC_PARAM_H__

#include "main.h"

#define PWM_TIM_CLOCK		168000000
#define PWM_TIM_FREQ		20000 /*Hz*/
#define PWM_TIM_PERIOD		(PWM_TIM_CLOCK/PWM_TIM_FREQ/2)	
#define PWM_TIM_TPWM		(PWM_TIM_CLOCK/PWM_TIM_FREQ/2)

#define FOC_FREQ			PWM_TIM_FREQ

//#define Motor_8318
//#define Motor_5065
#define Motor_Wade
//#define Motor_Tmotor_U10
//#define Motor_LD2808
typedef enum
{
	FOC_OK,
	FOC_FAULT
}ErrorState;

#endif