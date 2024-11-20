#ifndef __HW_CONF_H__
#define __HW_CONF_H__

#include "main.h"

#define PWM_TIM_CLOCK			168000000
#define PWM_TIM_FREQ			20000 /*Hz*/
#define PWM_TIM_PERIOD			(PWM_TIM_CLOCK / PWM_TIM_FREQ / 2)	

#define FOC_FREQ				PWM_TIM_FREQ
#define FOC_PERIOD				1.0f / (float)FOC_FREQ
	
/*采样电阻值*/
#define SAMPLE_RES				0.001f
/*电流采样放大器增益*/
#define CURRENT_AMP_GAIN		20.0f
/*计算得到的电流增益系数*/
#define SAMPLE_CURR_FACTOR		(float)(3.3f / 4095.0f / CURRENT_AMP_GAIN / SAMPLE_RES)

/*MOS管死区(ns)*/
#define MOS_DEADTIME			400.0f

/*母线电压采集分压电阻阻值*/
#define VBUS_R1					10000.0f
#define VBUS_R2					1000.0f

/*电流环周期(s)*/
#define Current_Ts	0.00005f
/*速度环周期(s)*/
#define Speed_Ts	0.0001f
/*位置环周期(s)*/
#define Position_Ts 0.0002f

#endif