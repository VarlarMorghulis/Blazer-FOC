#include "foc_utils.h"

//
/**
   * @brief  角度归一化函数,使输出角度在0-2PI范围内
   * @param  angle 原始角度
   * @retval 归一化后的角度
   */
float _normalizeAngle(float angle)
{
	float a;
	int a_int;
	
	a_int=(int)(angle/_2PI);
	a=angle-a_int*_2PI;
	
	return a>=0?a:(a+_2PI);
}

/**
   * @brief  一阶互补滤波函数
   * @param  ka(0-1)滤波系数,Ka越小,滤波效果越强,相位迟滞越大
			 *sample 当前滤波数据指针 *sample_last 上一次滤波数据指针 
   * @retval 滤波后的数据
   */
float LowPassFilter_Handle(float ka,float *sample,float *sample_last)
{
	float output;
	
	*sample=ka*(*sample)+(1-ka)*(*sample_last);
	*sample_last=*sample;
	
	output=*sample;
	
	return output;
}

/**
   * @brief  IIR数字滤波函数 使用二阶巴特沃斯滤波
   * @param  input 待滤波变量 滤波器结构体指针 
   * @retval output 滤波后输出变量
   */
float IIR_Butterworth_Handle(float input,IIR_Butterworth_TypeDef * IIR_Butterworth_t)
{
	float output;
	
	IIR_Butterworth_t->state_n = IIR_Butterworth_t->gain0 * input
								-IIR_Butterworth_t->a1 * IIR_Butterworth_t->state_n_1
								-IIR_Butterworth_t->a2 * IIR_Butterworth_t->state_n_2;
	
	output = IIR_Butterworth_t->gain1 * (IIR_Butterworth_t->b0 * IIR_Butterworth_t->state_n
									    +IIR_Butterworth_t->b1 * IIR_Butterworth_t->state_n_1
										+IIR_Butterworth_t->b2 * IIR_Butterworth_t->state_n_2);
	
	IIR_Butterworth_t->state_n_2=IIR_Butterworth_t->state_n_1;
	IIR_Butterworth_t->state_n_1=IIR_Butterworth_t->state_n;
	
	return output;
}


