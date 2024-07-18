#include "foc_identification.h"

TaskElement_TypeDef TE_Rs_Identification_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

extern FOC_State FOC_State_t;

enum {Ident_Rs,Ident_Ls,Ident_Flux};
uint8_t Ident_State=Ident_Rs;

float Motor_Rs;

/**
   * @brief  相电阻辨识任务
   * @param  无
   * @retval 无
   */
void FOC_Task_Rs_Identification(void)
{
	/*A相上管开通占空比*/
	static float duty=0.0f;
	/*等效电压*/
	static float U_sum;
	static float Uav;
	static float U_gain=3.3f/4095.0f*11.0f;
	/*等效电流*/
	static float I_sum;
	static float Iav;
	static float I_gain=3.3f/4095.0f/20.0f/0.001f;
	
	U_sum+=(float)ADC1->JDR4*U_gain*duty;
	I_sum+=((float)(2048-(int16_t)ADC1->JDR1))*I_gain;
	
	/*20ms执行一次*/
	if(++TE_Rs_Identification_t.Cnt_20kHz>=800)
	{
		TE_Rs_Identification_t.Cnt_20kHz=0;
		
		/*均值滤波*/
		Uav=U_sum/800.0f;
		Iav=I_sum/800.0f;
		U_sum=0.0f;
		I_sum=0.0f;
		
		if(Iav<20.0f)
		{
			duty+=0.001f;
			TIM1->CCR1=(uint16_t)(4200.0f*duty);
			TIM1->CCR2=0;
			TIM1->CCR3=0;
		}
		/*达到设定电流,计算相电阻*/
		else
		{
			TIM1->CCR1=4200;
			TIM1->CCR2=4200;
			TIM1->CCR3=4200;
			Motor_Rs=Uav/Iav*2.0f/3.0f;
			/*状态跳转*/
			FOC_State_t=FOC_Wait;
		}
		
		/*占空比给满,但还未达到设定电流*/
		if(duty>=1)
		{
			duty=0.0f;
			TIM1->CCR1=4200;
			TIM1->CCR2=4200;
			TIM1->CCR3=4200;
			/*状态跳转*/
			FOC_State_t=FOC_Error;
		}
	}

		
}

/**
   * @brief  相电感辨识任务
   * @param  无
   * @retval 无
   */
void FOC_Task_Ls_Identification(void)
{
	
}

/**
   * @brief  磁链辨识任务
   * @param  无
   * @retval 无
   */
void FOC_Task_Flux_Identification(void)
{
	
}

/**
   * @brief  参数辨识任务二级状态机
   * @param  无
   * @retval 无
   */
void FOC_Task_Identification(void)
{
	switch(Ident_State)
	{
		case Ident_Rs:
			FOC_Task_Rs_Identification();
		break;
		
		case Ident_Ls:
			FOC_Task_Ls_Identification();
		break;
			
		case Ident_Flux:
		{	
			FOC_Task_Flux_Identification();
		}
		break;
		
		default:break;
	}
}
