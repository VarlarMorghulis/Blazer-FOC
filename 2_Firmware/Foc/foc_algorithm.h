#ifndef __FOC_ALGORITHM_H__
#define __FOC_ALGORITHM_H__

#include "main.h"

typedef struct
{
	/*母线电压*/
	float Udc;
	/*abc三相电压*/
	float Ua,Ub,Uc;
	/*alpha beta轴电压*/
	float Ualpha,Ubeta;
	/*dq轴电压*/
	float Ud,Uq;
	/*abc三相电流*/
	float Ia,Ib,Ic;
	/*alpha beta轴电流*/
	float Ialpha,Ibeta;
	/*dq轴电流*/
	float Id,Iq;
	/*dq轴电流的低频成分*/
	float Id_l,Iq_l;
	/*dq轴电流的高频成分*/
	float Id_h,Iq_h;
	float Speed;
	float Speed_now;
	float Position;
	/*电角度过渡变量*/
	float theta_temp;
	/*电角度*/
	float theta_e;
	float Tcmp1,Tcmp2,Tcmp3,Tpwm;
	/*扇区*/
	uint8_t sector;
}FOC_TypeDef;

#include <arm_math.h> 
#include "foc_calibration.h"
#include "foc_param.h"

void Park_Transform(FOC_TypeDef *FOC_x);
void I_Park_Transform(FOC_TypeDef *FOC_x);
void Clarke_Transform(FOC_TypeDef *FOC_x);
void SVPWM_Cal(FOC_TypeDef *FOC_x);
void SetPWM(FOC_TypeDef *FOC_x);
ErrorState Current_Cal(FOC_TypeDef *FOC_t,CurrentOffset_TypeDef *CurrentOffset_t);
void Motor_Release(void);
#endif