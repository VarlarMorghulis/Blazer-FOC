#ifndef __FOC_ALGORITHM_H__
#define __FOC_ALGORITHM_H__

#include "main.h"

typedef struct
{
	/*ĸ�ߵ�ѹ*/
	float Udc;
	/*abc�����ѹ*/
	float Ua,Ub,Uc;
	/*alpha beta���ѹ*/
	float Ualpha,Ubeta;
	/*dq���ѹ*/
	float Ud,Uq;
	/*abc�������*/
	float Ia,Ib,Ic;
	/*alpha beta�����*/
	float Ialpha,Ibeta;
	/*dq�����*/
	float Id,Iq;
	/*dq������ĵ�Ƶ�ɷ�*/
	float Id_l,Iq_l;
	/*dq������ĸ�Ƶ�ɷ�*/
	float Id_h,Iq_h;
	float Speed;
	float Speed_now;
	float Position;
	/*��Ƕȹ��ɱ���*/
	float theta_temp;
	/*��Ƕ�*/
	float theta_e;
	float Tcmp1,Tcmp2,Tcmp3,Tpwm;
	/*����*/
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