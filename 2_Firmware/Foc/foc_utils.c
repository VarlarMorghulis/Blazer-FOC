#include "foc_utils.h"

/**
   * @brief  限幅函数
   * @param  amt 待限幅的变量 low 下限 high上限 
   * @retval 限幅后的值
   */
float _constrain(float amt,float low,float high)
{
	return ((amt<low)?(low):((amt>high)?high:amt));
}

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
	a = angle - a_int * _2PI;
	
	return a>=0 ? a : (a+_2PI);
}

/**
   * @brief  将浮点数转换为uint32_t
             此转换方法不会丢失精度
   * @param  x 待转换的浮点数
   * @retval *pInt 转换完的uint32_t
   */
uint32_t FloatToIntBit(float x)
{
	uint32_t *pInt;
	pInt=(uint32_t*)(&x);
	return *pInt;
}

/**
   * @brief  将uint32_t转换为float
             此转换方法不会丢失精度
   * @param  x 待转换的uint32_t
   * @retval *pFloat 转换完的浮点数
   */
float IntBitToFloat(uint32_t x)
{
	float * pFloat;
	pFloat=(float *)(&x);
	return *pFloat;
}

/**
   * @brief  IIR数字滤波函数 使用二阶巴特沃斯滤波
   * @param  input 待滤波变量 滤波器结构体指针 
   * @retval output 滤波后输出变量
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
   * @brief  快速反正切函数
   * @param  y x
   * @retval angle 角度值(弧度制)
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




