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
   * @brief  ��������ת��Ϊuint32_t
             ��ת���������ᶪʧ����
   * @param  x ��ת���ĸ�����
   * @retval *pInt ת�����uint32_t
   */
uint32_t FloatToIntBit(float x)
{
	uint32_t *pInt;
	pInt=(uint32_t*)(&x);
	return *pInt;
}

/**
   * @brief  ��uint32_tת��Ϊfloat
             ��ת���������ᶪʧ����
   * @param  x ��ת����uint32_t
   * @retval *pFloat ת����ĸ�����
   */
float IntBitToFloat(uint32_t x)
{
	float * pFloat;
	pFloat=(float *)(&x);
	return *pFloat;
}

/**
   * @brief  IIR�����˲����� ʹ�ö��װ�����˹�˲�
   * @param  input ���˲����� �˲����ṹ��ָ�� 
   * @retval output �˲����������
   */
float IIR_Butterworth(float input,IIR_Butterworth_TypeDef * IIR_Butterworth_t)
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

float fast_sq(float x)
{
	return x*x;
}

float fast_max(float x,float y)
{
	return (((x) > (y)) ? (x) : (y));
}

float fast_min(float x,float y)
{
	return (((x) < (y)) ? (x) : (y));
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




