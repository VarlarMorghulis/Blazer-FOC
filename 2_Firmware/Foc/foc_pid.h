#ifndef __FOC_PID_H__
#define __FOC_PID_H__

#include "foc_utils.h"

/*����������*/
#define Current_Ts	0.00005f

/*�ٶȻ�����*/
#define Speed_Ts	0.0001f

typedef struct
{
	/*Ŀ��ֵ ����ֵ*/
	float ref_value,samp_value;
	/*Kp Ki Kd*/
	float Kp,Ki,Kd;
	/*��ǰ��� ��һ����� �ۼ����*/
	float error,error_last,error_sum;
	/*�������������ֵ (����ֵ)*/
	float output_max;
	/*�����*/
	float output;
}PID_TypeDef;

float Current_PI_Ctrl(PID_TypeDef *PID);
float Speed_PI_Ctrl(PID_TypeDef *PID);
float Position_P_Ctrl(PID_TypeDef *PID);

#endif