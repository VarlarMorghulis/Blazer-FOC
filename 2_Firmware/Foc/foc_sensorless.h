#ifndef __FOC_SENSORLESS_H__
#define __FOC_SENSORLESS_H__

#include "common_inc.h"

/*高频正弦电压频率(Hz)*/
#define HFI_VOLT_FREQ 	500
/*计数变量*/
#define HFI_CNT_FLAG 	10000/HFI_VOLT_FREQ

/*锁相环积分周期*/
#define PLL_PERIOD		0.0001f

typedef struct
{
	/*高频注入电压幅值*/
	float inj_volt_amp;
	/*高频注入电压*/
	float inj_volt;
	/*正弦信号*/
	float sin_val;
	/*生成高频电压的计数变量*/
	uint32_t cnt;
	/*电角度*/
	float theta_el;
	/*电角速度*/
	float w_el;
}HFI_TypeDef;

typedef struct
{
	/*磁链观测器输入*/
	float Ualpha,Ubeta;
	float Ialpha,Ibeta;
	/*磁链观测器输出*/
	float sin,cos;
	/*磁链增益*/
	float gamma;
	/*算法执行周期*/
	float Ts;
	/*过渡变量*/
	float y1_last,y2_last;
	float etax1,etax2;
	float phi_err;
	float x1_last,x2_last;
	float x1,x2;
	float theta;
}Fluxobserver_TypeDef;

typedef struct
{
	/*误差和误差积分*/
	float error,error_sum;
	/*Kp Ki*/
	float Kp,Ki;
	/*误差积分限幅*/
	float error_sum_max;
	/*输出量*/
	float output;
	/*输出量积分*/
	float output_sum;
}PLL_TypeDef;

void Generate_HighFrequency_Volt(HFI_TypeDef * HFI_t);
void PLL_Handle(PLL_TypeDef * PLL_t);
void HFI_Process(void);
void Fluxobserver_Process(void);
#endif