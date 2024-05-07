#include "foc_utils.h"

//
/**
   * @brief  �Ƕȹ�һ������,ʹ����Ƕ���0-2PI��Χ��
   * @param  angle ԭʼ�Ƕ�
   * @retval ��һ����ĽǶ�
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
   * @brief  һ�׻����˲�����
   * @param  ka(0-1)�˲�ϵ��,KaԽС,�˲�Ч��Խǿ,��λ����Խ��
			 *sample ��ǰ�˲�����ָ�� *sample_last ��һ���˲�����ָ�� 
   * @retval �˲��������
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
   * @brief  IIR�����˲����� ʹ�ö��װ�����˹�˲�
   * @param  input ���˲����� �˲����ṹ��ָ�� 
   * @retval output �˲����������
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


