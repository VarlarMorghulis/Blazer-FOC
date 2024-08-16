#include "foc_identification.h"

TaskElement_TypeDef TE_Rs_Identification_t=
{
	.Step=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Ls_Identification_t=
{
	.Step=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

FOC_TypeDef FOC_Ls_Identification_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
};

extern FOC_State FOC_State_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Motor_TypeDef Motor_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern AnalogParam_TypeDef AnalogParam_t;
enum {Ident_Rs,Ident_Ls,Ident_Flux};
uint8_t Ident_State=Ident_Ls;

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
	
	/*100ms执行一次*/
	if(++TE_Rs_Identification_t.Cnt_20kHz>=2000)
	{
		TE_Rs_Identification_t.Cnt_20kHz=0;
		
		/*均值滤波*/
		Uav=U_sum/2000.0f;
		Iav=I_sum/2000.0f;
		U_sum=0.0f;
		I_sum=0.0f;
		
		if(Iav<15.0f)
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
			Motor_Rs=Uav/Iav*2.0f/3.0f-0.018f;
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

float Motor_Ls;
float sin_func[20]=
{0.0f, 0.309017f, 0.587785f, 0.809017f, 0.951057f, 1.0f, 0.951057f, 0.809017f, 0.587785f, 0.309017f,
 0.0f,-0.309017f,-0.587785f,-0.809017f,-0.951057f,-1.0f,-0.951057f,-0.809017f,-0.587785f,-0.309017f};
 
float Iq_Array[128]={0};
float FFTInput_Array[256]={0};
float FFTOutput_Array[64]={0};
float Iq_Amplitude[2]={0};

/**
   * @brief  相电感辨识任务
   * @param  无
   * @retval 无
   */
void FOC_Task_Ls_Identification(void)
{
	ErrorState Encoder_Ers,Current_Ers;
	static uint16_t i,j,k,l,m;
	
	float Upeak,Ipeak;
	
	FOC_StructBind(&FOC_Ls_Identification_t);
	
	Upeak=0.1f*AnalogParam_t.vbus;
	/*注入1kHz的Uq正弦波*/
	FOC_Ls_Identification_t.Uq=0.1f * sin_func[i];
	i++;
	if(i>=20)
		i=0;
	
		/*电流采样及处理*/
		Current_Ers=Current_Cal(&FOC_Ls_Identification_t,&CurrentOffset_t);
		/*Clarke变换*/
		Clarke_Transform(&FOC_Ls_Identification_t);
		/*Park变换*/
		Park_Transform(&FOC_Ls_Identification_t);
	
	/*获取编码器角度*/
	Encoder_Ers=Encoder_Cal(&FOC_Ls_Identification_t,&SPI_Encoder_t,Motor_t.Pole_Pairs);
	/*反Park变换*/
	I_Park_Transform(&FOC_Ls_Identification_t);
	/*SVPWM计算*/
	SVPWM_Cal(&FOC_Ls_Identification_t);
	/*占空比设置*/
	SetPWM(&FOC_Ls_Identification_t);
	
	/*等待一段时间,直到电压和电流都达到稳态*/
	if(TE_Ls_Identification_t.Step==0)
	{
		if(++TE_Ls_Identification_t.Cnt_20kHz>=40000)
		{
			TE_Ls_Identification_t.Step=1;
			TE_Ls_Identification_t.Cnt_20kHz=0;
		}	
	}
	/*Iq电流采样*/
	else if(TE_Ls_Identification_t.Step==1)
	{
		Iq_Array[j]=FOC_Ls_Identification_t.Iq;
		
		if(++j>=128)
		{
			TE_Ls_Identification_t.Step=2;
			j=0;
		}

	}
	/*数据处理*/
	else if(TE_Ls_Identification_t.Step==2)
	{
		for(k=0;k<128;k++)
		{
			/*实部赋值*/
			FFTInput_Array[2*k]=Iq_Array[k];
			/*虚部赋值*/
			FFTInput_Array[2*k+1]=0.0f;
		}
		/*傅里叶变换*/
		arm_cfft_f32(&arm_cfft_sR_f32_len128,FFTInput_Array,0,1);
		/*从0-20000Hz等间距提取128个点的幅值*/
		arm_cmplx_mag_f32(FFTInput_Array,FFTOutput_Array,128);
		for(l=0;l<64;l++)
			FFTOutput_Array[l]/=64.0f;
		/*筛选出最大的幅值*/
		for(l=0;l<10;l++)
		{
			Iq_Amplitude[0]=FFTOutput_Array[1+l];
			if(Iq_Amplitude[1]<Iq_Amplitude[0])
				Iq_Amplitude[1]=Iq_Amplitude[0];
		}
		Ipeak=Iq_Amplitude[1];
		Motor_Ls=Upeak/Ipeak/(_2PI*1000.0f);
		TE_Ls_Identification_t.Step=3;
	}
	else if(TE_Ls_Identification_t.Step==3)
	{
		/*重置变量*/
		TE_Ls_Identification_t.Step=0;
		i=0;
		j=0;
		k=0;
		l=0;
		m=0;
		/*状态跳转*/
		FOC_State_t=FOC_Wait;
	}
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
			FOC_Task_Flux_Identification();
		break;
		
		default:break;
	}
}
