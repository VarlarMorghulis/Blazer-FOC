#ifndef __FOC_ALGORITHM_H__
#define __FOC_ALGORITHM_H__

#include "main.h"
#include <arm_math.h> 
#include "foc_utils.h"
#include "hw_conf.h"
#include "data_type.h"

typedef struct
{
	float Vbus,Vbus_filt;
	float Ibus,Ibus_filt;
	float Power_filt;
	/*abc三相电压*/
	float Va,Vb,Vc;
	/*alpha beta轴电压*/
	float Valpha,Vbeta;
	/*dq轴电压*/
	float Vd,Vq;
	/*abc三相电流*/
	float Ia,Ib,Ic;
	/*alpha beta轴电流*/
	float Ialpha,Ibeta;
	/*dq轴电流*/
	float Id,Iq,Id_filt,Iq_filt;
	float Vd_int,Vq_int;
	float mod_d,mod_q;
	float mod_alpha,mod_beta;
	/*dq轴电流的低频成分*/
	float dtc_a,dtc_b,dtc_c;
	/*扇区*/
	int32_t sector;
	
}FOC_TypeDef;

void Park_Transform(float Ialpha, float Ibeta, float theta, float *Id, float *Iq);
void Inverse_Park_Transform(float mod_d, float mod_q, float theta, float *mod_alpha, float *mod_beta);
void Clarke_Transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta);
void FOC_SVM(float alpha, float beta, float *tA, float *tB, float *tC ,int32_t *sector);
void FOC_Voltage(float Vd_set, float Vq_set, float phase);
void FOC_Current(float Id_set, float Iq_set, float phase, float phase_vel);

void PWM_TurnOnHighSides(void);
void PWM_TurnOnLowSides(void);
#endif