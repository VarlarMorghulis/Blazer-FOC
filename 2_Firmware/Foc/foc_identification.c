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
   * @brief  ������ʶ����
   * @param  ��
   * @retval ��
   */
void FOC_Task_Rs_Identification(void)
{
	/*A���Ϲܿ�ͨռ�ձ�*/
	static float duty=0.0f;
	/*��Ч��ѹ*/
	static float U_sum;
	static float Uav;
	static float U_gain=3.3f/4095.0f*11.0f;
	/*��Ч����*/
	static float I_sum;
	static float Iav;
	static float I_gain=3.3f/4095.0f/20.0f/0.001f;
	
	U_sum+=(float)ADC1->JDR4*U_gain*duty;
	I_sum+=((float)(2048-(int16_t)ADC1->JDR1))*I_gain;
	
	/*20msִ��һ��*/
	if(++TE_Rs_Identification_t.Cnt_20kHz>=800)
	{
		TE_Rs_Identification_t.Cnt_20kHz=0;
		
		/*��ֵ�˲�*/
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
		/*�ﵽ�趨����,���������*/
		else
		{
			TIM1->CCR1=4200;
			TIM1->CCR2=4200;
			TIM1->CCR3=4200;
			Motor_Rs=Uav/Iav*2.0f/3.0f;
			/*״̬��ת*/
			FOC_State_t=FOC_Wait;
		}
		
		/*ռ�ձȸ���,����δ�ﵽ�趨����*/
		if(duty>=1)
		{
			duty=0.0f;
			TIM1->CCR1=4200;
			TIM1->CCR2=4200;
			TIM1->CCR3=4200;
			/*״̬��ת*/
			FOC_State_t=FOC_Error;
		}
	}

		
}

/**
   * @brief  ���б�ʶ����
   * @param  ��
   * @retval ��
   */
void FOC_Task_Ls_Identification(void)
{
	
}

/**
   * @brief  ������ʶ����
   * @param  ��
   * @retval ��
   */
void FOC_Task_Flux_Identification(void)
{
	
}

/**
   * @brief  ������ʶ�������״̬��
   * @param  ��
   * @retval ��
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
