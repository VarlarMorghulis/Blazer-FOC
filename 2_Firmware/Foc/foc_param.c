#include "foc_param.h"

InterfaceParam_TypeDef InterfaceParam_t;

extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_Position;

void FOC_Param_Init(void)
{
	InterfaceParam_t.current_max=40.0f;
	PID_Speed.output_max=InterfaceParam_t.current_max;
	
	//InterfaceParam_t.accup_max=
}

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
   * @brief  将当前运行的FOC结构体指针指向FOC_Param_t 
   * @param  FOC结构体指针
   * @retval 无
   */
void FOC_StructBind(FOC_TypeDef *FOC_t)
{
	FOC_Param_t=FOC_t;
}

/**
   * @brief  将空闲FOC结构体指针指向FOC_Param_t 
   * @param  无
   * @retval 无
   */
void FOC_StructUnbind(void)
{
	FOC_Param_t=&FOC_Idle_t;
}

