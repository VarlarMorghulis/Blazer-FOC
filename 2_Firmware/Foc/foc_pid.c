#include "foc_pid.h"

/*��������(A)*/
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
	
	PID->error_sum = _constrain(PID->error_sum,-1.0f,1.0f);
	
	PID->output = PID->Kp * PID->error + PID->error_sum;
	
	PID->output= _constrain(PID->output,-1.0f,1.0f);
	
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
	
	PID->error_sum = _constrain(PID->error_sum,-1.0f,1.0f);
	
	if(PID->error > 0.5f * PID->ref_value || PID->error < -0.5f * PID->ref_value)
		PID->error_sum=0.0f;
	
	PID->output = PID->Kp * PID->error + PID->error_sum;
	
	PID->output= _constrain(PID->output,-1.0f,1.0f);
	
	return PID->output * PID->output_max;
}

/**
   * @brief  λ�û� ʹ��P���� λ��ʽ
   * @param  PID�ṹ���ַ
   * @retval ���Ϊ�ٶ�
   */
float Position_P_Ctrl(PID_TypeDef *PID)
{
	PID->error = PID->ref_value - PID->samp_value;
	
	PID->error_sum += PID->error * PID->Ki * Position_Ts;
	
	PID->error_sum = _constrain(PID->error_sum,-1.0f,1.0f);
	
	PID->output = PID->Kp * PID->error;
	
	PID->output= _constrain(PID->output,-1.0f,1.0f);

	return PID->output * PID->output_max;
}

void Clear_PID_Param(PID_TypeDef *PID)
{
	PID->ref_value=0.0f;
	PID->error=0.0f;
	PID->output=0.0f;
	PID->error_sum=0.0f;
}