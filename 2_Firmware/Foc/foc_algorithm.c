#include "foc_algorithm.h"

/**
   * @brief  Park变换
   * @param  FOC结构体指针
   * @retval 无
   */
void Park_Transform(FOC_TypeDef *FOC_t)
{
	FOC_t->Id =  FOC_t->Ialpha * arm_cos_f32(FOC_t->theta_e) + FOC_t->Ibeta * arm_sin_f32(FOC_t->theta_e);
	FOC_t->Iq = -FOC_t->Ialpha * arm_sin_f32(FOC_t->theta_e) + FOC_t->Ibeta * arm_cos_f32(FOC_t->theta_e);
}

/**
   * @brief  Park逆变换
   * @param  FOC结构体指针
   * @retval 无
   */
void I_Park_Transform(FOC_TypeDef *FOC_t)
{
	FOC_t->Ualpha = FOC_t->Ud * arm_cos_f32(FOC_t->theta_e) - FOC_t->Uq * arm_sin_f32(FOC_t->theta_e);
	FOC_t->Ubeta  = FOC_t->Ud * arm_sin_f32(FOC_t->theta_e) + FOC_t->Uq * arm_cos_f32(FOC_t->theta_e);
}


/**
   * @brief  Clarke变换
   * @param  FOC结构体指针
   * @retval 无
   */
void Clarke_Transform(FOC_TypeDef *FOC_t)
{
	FOC_t->Ialpha =  FOC_t->Ia;
	FOC_t->Ibeta  = (FOC_t->Ia + 2.0f * FOC_t->Ib) / _SQRT_3;
}

/**
   * @brief  SVPWM计算模块
   * @param  FOC结构体指针
   * @retval 无
   */
void SVPWM_Cal(FOC_TypeDef *FOC_t)
{
	uint8_t sector_temp=0;
	float Uref1,Uref2,Uref3;
	float X,Y,Z;
	float Tx=0,Ty=0;
	float Ta=0,Tb=0,Tc=0;
	
	Uref1 = FOC_t->Ubeta;
	Uref2 = ( _SQRT_3 * FOC_t->Ualpha - FOC_t->Ubeta) / 2.0f;
	Uref3 = (-_SQRT_3 * FOC_t->Ualpha - FOC_t->Ubeta) / 2.0f;
	X = _SQRT_3 * FOC_t->Tpwm *   Uref1  / FOC_t->Udc;
	Y = _SQRT_3 * FOC_t->Tpwm * (-Uref3) / FOC_t->Udc;
	Z = _SQRT_3 * FOC_t->Tpwm * (-Uref2) / FOC_t->Udc;

	/*1.扇区判断*/
	if(Uref1>0)
		sector_temp=1;
	if(Uref2>0)
		sector_temp+=2;
	if(Uref3>0)
		sector_temp+=4;

	switch(sector_temp)
	{
		case 1: FOC_t->sector = 2; break;
		case 2: FOC_t->sector = 6; break;	
		case 3: FOC_t->sector = 1; break;
		case 4: FOC_t->sector = 4; break;
		case 5: FOC_t->sector = 3; break;
		case 6: FOC_t->sector = 5; break;
		default:break;
	}
	
	/*2.矢量作用时长计算*/
	switch(FOC_t->sector)
	{
		case 1: Tx =-Z; Ty = X; break;
		case 2: Tx = Z; Ty = Y; break;	
		case 3: Tx = X; Ty =-Y; break;
		case 4: Tx =-X; Ty = Z; break;
		case 5: Tx =-Y; Ty =-Z; break;
		case 6: Tx = Y; Ty =-X; break;
		default:break;
	}
	
	/*3.限幅处理*/
	if(Tx+Ty>FOC_t->Tpwm)
	{
		Tx = Tx / (Tx + Ty) * FOC_t->Tpwm;
		Ty = Ty / (Tx + Ty) * FOC_t->Tpwm;
	}
	
	/*4.定时器比较值计算*/
	Ta = (FOC_t->Tpwm - Tx - Ty) / 4.0f;
	Tb = (FOC_t->Tpwm + Tx - Ty) / 4.0f;
	Tc = (FOC_t->Tpwm + Tx + Ty) / 4.0f;
	
	switch(FOC_t->sector)
	{
		case 1: FOC_t->Tcmp1 = Ta; FOC_t->Tcmp2 = Tb; FOC_t->Tcmp3 = Tc; break;
		case 2: FOC_t->Tcmp1 = Tb; FOC_t->Tcmp2 = Ta; FOC_t->Tcmp3 = Tc; break;	
		case 3: FOC_t->Tcmp1 = Tc; FOC_t->Tcmp2 = Ta; FOC_t->Tcmp3 = Tb; break;
		case 4: FOC_t->Tcmp1 = Tc; FOC_t->Tcmp2 = Tb; FOC_t->Tcmp3 = Ta; break;
		case 5: FOC_t->Tcmp1 = Tb; FOC_t->Tcmp2 = Tc; FOC_t->Tcmp3 = Ta; break;
		case 6: FOC_t->Tcmp1 = Ta; FOC_t->Tcmp2 = Tc; FOC_t->Tcmp3 = Tb; break;
		default:break;
	}
}

/**
   * @brief  PWM赋值 改变定时器的CCR寄存器值
   * @param  FOC结构体指针
   * @retval 无
   */
void SetPWM(FOC_TypeDef *FOC_t)
{
	TIM1->CCR1=(uint16_t)FOC_t->Tcmp1;
	TIM1->CCR2=(uint16_t)FOC_t->Tcmp2;
	TIM1->CCR3=(uint16_t)FOC_t->Tcmp3;
}

/*采样电阻值*/
#define SAMPLE_RES				0.001f
/*电流采样放大器增益*/
#define AMP_GAIN				20.0f
/*计算得到的电流增益系数*/
#define SAMPLE_CURR_FACTOR		(float)(3.3f/4095.0f/AMP_GAIN/SAMPLE_RES)
/**
   * @brief  电流采样计算函数
   * @param  FOC结构体指针
   * @retval 无
   */
ErrorState Current_Cal(FOC_TypeDef *FOC_t,CurrentOffset_TypeDef *CurrentOffset_t)
{
	/*电流采样偏置在一个合理范围内(2048+-100),否则报错*/
	if(CurrentOffset_t->A_Offset<1948||CurrentOffset_t->A_Offset>2148 ||
	   CurrentOffset_t->B_Offset<1948||CurrentOffset_t->B_Offset>2148 ||
	   CurrentOffset_t->C_Offset<1948||CurrentOffset_t->C_Offset>2148	)
	{
		return FOC_FAULT;
	}
	
	else
	{	
		FOC_t->Ia = ((float)((int16_t)ADC1->JDR1 - CurrentOffset_t->A_Offset)) * SAMPLE_CURR_FACTOR;
		FOC_t->Ib = ((float)((int16_t)ADC1->JDR2 - CurrentOffset_t->B_Offset)) * SAMPLE_CURR_FACTOR;
		FOC_t->Ic = - FOC_t->Ia - FOC_t->Ib;
		//FOC_t->Ic=((float)((int16_t)ADC1->JDR3-CurrentOffset_t->C_Offset))*SAMPLE_CURR_FACTOR;
		return FOC_OK;
	}
}

/**
   * @brief  电机停转设置函数
   * @param  无
   * @retval 无
   */
void Motor_Release(void)
{
	TIM1->CCR1=4200;
	TIM1->CCR2=4200;
	TIM1->CCR3=4200;
} 

/**
   * @brief  电机刹车设置函数
   * @param  无
   * @retval 无
   */
void Motor_Brake(void)
{
	TIM1->CCR1=0;
	TIM1->CCR2=0;
	TIM1->CCR3=0;
} 