#ifndef __FOC_HFI_H__
#define __FOC_HFI_H__

#include "common_inc.h"

/*��Ƶ���ҵ�ѹƵ��(Hz)*/
#define HFI_VOLT_FREQ 	500
/*��������*/
#define HFI_CNT_FLAG 	10000/HFI_VOLT_FREQ

/*���໷��������*/
#define PLL_PERIOD		0.0001f

typedef struct
{
	/*��Ƶע���ѹ��ֵ*/
	float inj_volt_amp;
	/*��Ƶע���ѹ*/
	float inj_volt;
	/*�����ź�*/
	float sin_val;
	/*���ɸ�Ƶ��ѹ�ļ�������*/
	uint32_t cnt;
	/*��Ƕ�*/
	float theta_el;
	/*����ٶ�*/
	float w_el;
}HFI_TypeDef;

typedef struct
{
	/*����������*/
	float error,error_sum;
	/*Kp Ki*/
	float Kp,Ki;
	/*�������޷�*/
	float error_sum_max;
	/*�����*/
	float output;
	/*���������*/
	float output_sum;
}PLL_TypeDef;

void Generate_HighFrequency_Volt(HFI_TypeDef * HFI_t);
void PLL_Handle(PLL_TypeDef * PLL_t);
void HFI_Process(void);

#endif