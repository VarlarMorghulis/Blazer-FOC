#ifndef __FOC_UTILS_H__
#define __FOC_UTILS_H__

#include "main.h"

#define _PI 			3.1415926536f
#define _PI_2 			1.5707963268f
#define _PI_3 			1.0471975512f
#define _2PI 			6.2831853072f
#define _3PI_2 			4.7123889804f

#define _SQRT_3			1.732050807f
#define ONE_BY_SQRT_3 	0.577350269f
#define TWO_BY_SQRT_3	1.154700538f
#define SQRT_3_BY_2		0.866025403f

#define UTILS_LP_FAST(value, sample, filter_constant) (value -= (filter_constant) * ((value) - (sample)))

#define UTILS_LP_MOVING_AVG_APPROX(value, sample, N)  UTILS_LP_FAST(value, sample, 2.0f / ((N) + 1.0f))

typedef struct
{
	float state_n,state_n_1,state_n_2;
	float a0,a1,a2;
	float b0,b1,b2;
	float gain0,gain1;
}IIR_Butterworth_TypeDef;

float _constrain(float amt, float low, float high);
float _normalizeAngle(float angle);
uint32_t FloatToIntBit(float x);
float IntBitToFloat(uint32_t x);
float IIR_Butterworth(float input, IIR_Butterworth_TypeDef * IIR_Butterworth_t);
float fast_abs(float x);
float fast_sq(float x);
float fast_max(float x,float y);
float fast_min(float x,float y);
float fast_atan2(float y, float x);

#endif