#ifndef __FOC_PARAM_H__
#define __FOC_PARAM_H__

#include "main.h"

#define PWM_TIM_CLOCK		168000000
#define PWM_TIM_FREQ		20000 /*Hz*/
#define PWM_TIM_PERIOD		(PWM_TIM_CLOCK/PWM_TIM_FREQ/2)	
#define PWM_TIM_TPWM		(PWM_TIM_CLOCK/PWM_TIM_FREQ/2)

#define FOC_FREQ			PWM_TIM_FREQ
#define FOC_PERIOD			1.0f/(float)FOC_FREQ

//#define Motor_8318
//#define Motor_5065
#define Motor_Tmotor_U7
//#define Motor_Tmotor_U10
//#define Motor_LD2808
typedef enum
{
	FOC_OK,
	FOC_FAULT
}ErrorState;

typedef struct
{
	float node_id;
	float enc_type;
	float sensor_dir;
	float zero_enc_offset;
	float currentoffset_a,currentoffset_b,currentoffset_c;
	float pole_pairs;
	float current_max;
	float speed_max;
	float accup_max;
	float accdown_max;
}InterfaceParam_TypeDef;

#include "common_inc.h"

void Param_Return_Default(void);
void FOC_Param_Init(void);
void FOC_StructBind(FOC_TypeDef *FOC_t);
void FOC_StructUnbind(void);

#endif