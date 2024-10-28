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
float Motor_Ls;
float Motor_Flux;

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
	
	U_sum+=(float)(ADC1->JDR4)*U_gain*duty;
	I_sum+=((float)(CurrentOffset_t.A_Offset-(int16_t)ADC1->JDR1))*I_gain;
	
	/*100msִ��һ��*/
	if(++TE_Rs_Identification_t.Cnt_20kHz>=2000)
	{
		TE_Rs_Identification_t.Cnt_20kHz=0;
		
		/*��ֵ�˲�*/
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
		/*�ﵽ�趨����,���������*/
		else
		{
			TIM1->CCR1=4200;
			TIM1->CCR2=4200;
			TIM1->CCR3=4200;
			Motor_Rs=Uav/Iav*2.0f/3.0f-0.003f;
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


float sin_func[20]=
{0.0f, 0.309017f, 0.587785f, 0.809017f, 0.951057f, 1.0f, 0.951057f, 0.809017f, 0.587785f, 0.309017f,
 0.0f,-0.309017f,-0.587785f,-0.809017f,-0.951057f,-1.0f,-0.951057f,-0.809017f,-0.587785f,-0.309017f};
 
float Iq_Array[128]={0};
float FFTInput_Array[256]={0};
float FFTOutput_Array[64]={0};
float Iq_Amplitude[2]={0};

/**
   * @brief  ���б�ʶ����
   * @param  ��
   * @retval ��
   */
void FOC_Task_Ls_Identification(void)
{
	ErrorState Current_Ers=FOC_OK,Encoder_Ers=FOC_OK;
	static uint16_t i,j,k,l,m;
	
	float Upeak,Ipeak;
	
	FOC_StructBind(&FOC_Ls_Identification_t);
	
	Upeak=0.2f*AnalogParam_t.vbus;
	/*ע��1kHz��Uq���Ҳ�*/
	//FOC_Ls_Identification_t.Ud=0.2f * arm_sin_f32(_2PI/20.0f*(float)i);
	if(i%2==0)
		FOC_Ls_Identification_t.Uq=0.2f;
	else if(i%2==1)
		FOC_Ls_Identification_t.Uq=-0.2f;
	

	FOC_Ls_Identification_t.theta_e=0.0f;
	i++;
	if(i>=20)
		i=0;
	
	/*��������������*/
	Current_Ers=Current_Cal(&FOC_Ls_Identification_t,&CurrentOffset_t);
	/*Clarke�任*/
	//Clarke_Transform(&FOC_Ls_Identification_t);
	/*Park�任*/
	//Park_Transform(&FOC_Ls_Identification_t);
	
	/*��ȡ�������Ƕ�*/
	//Encoder_Ers=Encoder_Cal(&FOC_Ls_Identification_t,&SPI_Encoder_t,Motor_t.Pole_Pairs);

	/*��Park�任*/
	I_Park_Transform(&FOC_Ls_Identification_t);
	/*SVPWM����*/
	SVPWM_Cal(&FOC_Ls_Identification_t);
	/*ռ�ձ�����*/
	SetPWM(&FOC_Ls_Identification_t);
	
	/*�ȴ�һ��ʱ��,ֱ����ѹ�͵������ﵽ��̬*/
	if(TE_Ls_Identification_t.Step==0)
	{
		if(++TE_Ls_Identification_t.Cnt_20kHz>=60000)
		{
			TE_Ls_Identification_t.Step=1;
			TE_Ls_Identification_t.Cnt_20kHz=0;
		}	
	}
	/*Iq��������*/
	else if(TE_Ls_Identification_t.Step==1)
	{
		Iq_Array[j]=FOC_Ls_Identification_t.Iq;
		
		if(++j>=128)
		{
			TE_Ls_Identification_t.Step=2;
			j=0;
		}

	}
	/*���ݴ���*/
	else if(TE_Ls_Identification_t.Step==2)
	{
		for(k=0;k<128;k++)
		{
			/*ʵ����ֵ*/
			FFTInput_Array[2*k]=Iq_Array[k];
			/*�鲿��ֵ*/
			FFTInput_Array[2*k+1]=0.0f;
		}
		/*���ٸ���Ҷ�任*/
		arm_cfft_f32(&arm_cfft_sR_f32_len128,FFTInput_Array,0,1);
		/*��0-20000Hz�ȼ����ȡ128����ķ�ֵ*/
		arm_cmplx_mag_f32(FFTInput_Array,FFTOutput_Array,128);
		for(l=0;l<64;l++)
			FFTOutput_Array[l]/=64.0f;
		/*ɸѡ�����ķ�ֵ*/
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
		/*���ñ���*/
		TE_Ls_Identification_t.Step=0;
		i=0;
		j=0;
		k=0;
		l=0;
		m=0;
		/*״̬��ת*/
		FOC_State_t=FOC_Wait;
	}
	
	if(Current_Ers==FOC_FAULT||Encoder_Ers==FOC_FAULT)
	{
		FOC_State_t=FOC_Error;
	}	
}

//float Ia[2];
//float U_bus;
//float di_by_dt;
///**
//   * @brief  ���б�ʶ����
//   * @param  ��
//   * @retval ��
//   */
//void FOC_Task_Ls_Identification(void)
//{
//	float calib_voltage=0.1f;
//	static float Ud[2];
//	static float U_gain=3.3f/4095.0f*11.0f;
//	uint32_t num_L_cycles = FOC_FREQ;
//	
//	float L;
//	
//	int i=0;
//	
//	Ud[0]=-calib_voltage;
//	Ud[1]=+calib_voltage;
//	
//	FOC_StructBind(&FOC_Ls_Identification_t);
//	
//	/*���ٱ��ѹ*/
//	if(TE_Ls_Identification_t.Step==0)
//	{
//		i=TE_Ls_Identification_t.Cnt_20kHz & 1;
//		
//		Ia[i] += FOC_Ls_Identification_t.Ia;
//		U_bus += (float)(ADC1->JDR4)*U_gain;
//		
//		FOC_Ls_Identification_t.theta_e=0.0f;
//		FOC_Ls_Identification_t.Ud=Ud[i];
//		
//		I_Park_Transform(&FOC_Ls_Identification_t);
//		
//		SVPWM_Cal(&FOC_Ls_Identification_t);
//		
//		SetPWM(&FOC_Ls_Identification_t);
//		
//		Current_Cal(&FOC_Ls_Identification_t,&CurrentOffset_t);
//		
//		if(++TE_Ls_Identification_t.Cnt_20kHz>=(num_L_cycles<<1))
//		{
//			TE_Ls_Identification_t.Step=1;
//			TE_Ls_Identification_t.Cnt_20kHz=0;
//		}
//	}
//	/*��������*/
//	else if(TE_Ls_Identification_t.Step==1)
//	{
//		U_bus /= (float)(num_L_cycles*2);
//		di_by_dt = (Ia[1]-Ia[0])/(float)(FOC_PERIOD * num_L_cycles);
//		L = U_bus * calib_voltage / di_by_dt;
//		Motor_Ls = L *2.0f / 3.0f;
//		
////		U_bus=0.0f;
////		Ia[0]=0.0f;
////		Ia[1]=0.0f;
////		di_by_dt=0.0f;
//		
//		TE_Ls_Identification_t.Step=0;
//		FOC_State_t=FOC_Wait;
//	}
//}


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
			FOC_Task_Flux_Identification();
		break;
		
		default:break;
	}
}
