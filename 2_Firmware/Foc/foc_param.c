#include "foc_param.h"

FOC_TypeDef *FOC_Param_t;
FOC_TypeDef FOC_Idle_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
	.Tcmp1=4200,.Tcmp2=4200,.Tcmp3=4200
};

/**
   * @brief  ����ǰ���е�FOC�ṹ��ָ��ָ��FOC_Param_t 
   * @param  FOC�ṹ��ָ��
   * @retval ��
   */
void FOC_StructBind(FOC_TypeDef *FOC_t)
{
	FOC_Param_t=FOC_t;
}

/**
   * @brief  ������FOC�ṹ��ָ��ָ��FOC_Param_t 
   * @param  ��
   * @retval ��
   */
void FOC_StructUnbind(void)
{
	FOC_Param_t=&FOC_Idle_t;
}