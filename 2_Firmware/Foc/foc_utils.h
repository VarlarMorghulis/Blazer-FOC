#ifndef __FOC_UTILS_H__
#define __FOC_UTILS_H__

#define _PI 	3.1415926536f
#define _PI_2 	1.5707963268f
#define _PI_3 	1.0471975512f
#define _2PI 	6.2831853072f
#define _3PI_2 	4.7123889804f

/*ÏÞ·ùº¯Êý*/
#define _constrain(amt,low,high) ((amt<low)?(low):((amt>high)?high:amt))

typedef struct
{
	float state_n,state_n_1,state_n_2;
	float a0,a1,a2;
	float b0,b1,b2;
	float gain0,gain1;
}IIR_Butterworth_TypeDef;

float _normalizeAngle(float angle);
float LowPassFilter_Handle(float Ka,float *Sample,float *Sample_last);
float IIR_Butterworth_Handle(float input,IIR_Butterworth_TypeDef * IIR_Butterworth_t);

#endif