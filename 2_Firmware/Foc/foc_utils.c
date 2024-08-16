#include "foc_utils.h"

/**
   * @brief  �޷�����
   * @param  amt ���޷��ı��� low ���� high���� 
   * @retval �޷����ֵ
   */
float _constrain(float amt,float low,float high)
{
	return ((amt<low)?(low):((amt>high)?high:amt));
}

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
	a = angle - a_int * _2PI;
	
	return a>=0 ? a : (a+_2PI);
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
	
	IIR_Butterworth_t->state_n_2 = IIR_Butterworth_t->state_n_1;
	IIR_Butterworth_t->state_n_1 = IIR_Butterworth_t->state_n;
	
	return output;
}

float fast_abs(float x)
{
	return x >=0 ? x : -x;
}
/**
   * @brief  ���ٷ����к���
   * @param  y x
   * @retval angle �Ƕ�ֵ(������)
   */
float fast_atan2(float y, float x) 
{
    float abs_y = fast_abs(y) + 1e-20f; // kludge to prevent 0/0 condition
    float angle;

    if (x >= 0) {
        float r = (x - abs_y) / (x + abs_y);
        float rsq = r * r;
        angle = ((0.1963f * rsq) - 0.9817f) * r + (_PI / 4.0f);
    } else {
        float r = (x + abs_y) / (abs_y - x);
        float rsq = r * r;
        angle = ((0.1963f * rsq) - 0.9817f) * r + (3.0f * _PI / 4.0f);
    }

    if (y < 0) {
        return(-angle);
    } else {
        return(angle);
    }
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
	
	*sample = ka * (*sample) + (1-ka) * (*sample_last);
	*sample_last = *sample;
	
	output = *sample;
	
	return output;
}



