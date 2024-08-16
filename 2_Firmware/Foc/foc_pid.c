#include "foc_pid.h"

float dead_current=0.2f;
/**
   * @brief  ������ ʹ��PI���� λ��ʽ
   * @param  PID�ṹ���ַ
   * @retval ���Ϊ��ѹ
   */
float Current_PI_Ctrl(PID_TypeDef *PID)
{
	PID->error = PID->ref_value - PID->samp_value;
	
	if(PID->error < dead_current && PID->error > -PID->error)
		PID->error=0.0f;
	
	PID->error_sum += PID->error * PID->Ki * Current_Ts;
	
	PID->error_sum = _constrain(PID->error_sum,-PID->output_max,PID->output_max);
	
	PID->output = PID->Kp * PID->error + PID->error_sum;
	
	PID->output= _constrain(PID->output,-PID->output_max,PID->output_max);
	
	return PID->output;
}

/**
   * @brief  �ٶȻ� ʹ��PI���� λ��ʽ
   * @param  PID�ṹ���ַ
   * @retval ���Ϊq�����
   */
float Speed_PI_Ctrl(PID_TypeDef *PID)
{
	PID->error = PID->ref_value - PID->samp_value;
	
	PID->error_sum += PID->error * PID->Ki * Speed_Ts;
	
	PID->error_sum = _constrain(PID->error_sum,-PID->output_max,PID->output_max);
	
	if((PID->error >  0.3f * PID->ref_value && PID->error <  0.8f * PID->ref_value) ||
	   (PID->error < -0.3f * PID->ref_value && PID->error > -0.8f * PID->ref_value))
		PID->error_sum=0.0f;
	
	PID->output = PID->Kp * PID->error + PID->error_sum;
	
	PID->output = _constrain(PID->output,-PID->output_max,PID->output_max);
	
	return PID->output;
}

/**
   * @brief  λ�û� ʹ��P���� λ��ʽ
   * @param  PID�ṹ���ַ
   * @retval ���Ϊ�ٶ�
   */
float Position_P_Ctrl(PID_TypeDef *PID)
{
	PID->error = PID->ref_value - PID->samp_value;
	
//	if(PID->error<=0.01f&&PID->error>=-0.01f)//��������
//		PID->error=0;
	
	PID->output = PID->Kp * PID->error;
	
	PID->output= _constrain(PID->output,-PID->output_max,PID->output_max);

	return PID->output;
}
