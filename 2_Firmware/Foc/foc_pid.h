#ifndef __FOC_PID_H__
#define __FOC_PID_H__

#include "foc_utils.h"
#include "foc_run.h"
#include "hw_conf.h"

typedef struct
{
	/*目标值 反馈值*/
	float ref_value,samp_value;
	/*Kp Ki Kd*/
	float Kp,Ki,Kd;
	/*当前误差 上一次误差 累加误差*/
	float error,error_last,error_sum;
	/*输出量允许的最大值 (绝对值)*/
	float output_max;
	/*输出量*/
	float output;
}PID_TypeDef;

float Current_PI_Ctrl(PID_TypeDef *PID);
float Speed_PI_Ctrl(PID_TypeDef *PID);
float Position_P_Ctrl(PID_TypeDef *PID);
void Clear_PID_Param(PID_TypeDef *PID);

#endif