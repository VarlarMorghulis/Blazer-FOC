#include "foc_calibration.h"

uint16_t Anticog_1[2048];
uint16_t Anticog_2[2048];
uint16_t Anticog_3[2048];

FOC_TypeDef FOC_Encoder_Calibration_t=
{
	.Udc=24.0f,
	.Ud=2.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
};

FOC_TypeDef FOC_Anticogging_Calibration_t=
{
	.Udc=24.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
};

TaskElement_TypeDef TE_ADC_Calibration_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Encoder_Calibration_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Anticogging_Calibration_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Calib_Currentloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Calib_Speedloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

CurrentOffset_TypeDef CurrentOffset_t=
{
	.A_Offset=2048,
	.B_Offset=2048,
	.C_Offset=2048,
};

/*
PID_TypeDef PID_Id_Calib=
{
	.ref_value=0.0f,
	.Kp=0.25f,
	.Ki=145.7f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq_Calib=
{
	.ref_value=0.0f,
	.Kp=0.25f,
	.Ki=145.7f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};
*/
PID_TypeDef PID_Id_Calib=
{
	.ref_value=0.0f,
	.Kp=0.2985f,
	.Ki=644.38f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq_Calib=
{
	.ref_value=0.0f,
	.Kp=0.2985f,
	.Ki=644.38f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Speed_Calib=
{
	.ref_value=0.3141593f,
	.Kp=0.4f,
	.Ki=2.0f,
	.error_sum_max=5.0f,
	.output_max=8.0f
};

enum {Calib_ADC,Calib_Encoder,Calib_Anticogging,Calib_Done};
uint8_t Calib_State=Calib_ADC;
uint8_t Z_use_flag;
uint8_t Z_detect_flag;

extern FOC_State FOC_State_t;
extern Encoder_TypeDef TLE5012B_t;
extern Encoder_TypeDef ABZ_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern Motor_TypeDef Motor_t;


/**
   * @brief  ADC����������ƫУ׼���� ִ��Ƶ��Ϊ20kHz
   * @param  ��
   * @retval ��
   */
void FOC_Task_ADC_Calibration(void)
{
	static uint32_t A_offset_sum,B_offset_sum,C_offset_sum;
	
	if(++TE_ADC_Calibration_t.Cnt_20kHz<=10000)
	{
		A_offset_sum+=ADC1->JDR1;
		B_offset_sum+=ADC1->JDR2;
		C_offset_sum+=ADC1->JDR3;
	}
	else
	{
		CurrentOffset_t.A_Offset=A_offset_sum/10000;
		CurrentOffset_t.B_Offset=B_offset_sum/10000;
		CurrentOffset_t.C_Offset=C_offset_sum/10000;
		
		A_offset_sum=0;
		B_offset_sum=0;
		C_offset_sum=0;
		
		TE_ADC_Calibration_t.Cnt_20kHz=0;
		
		/*����������ƫ��һ������Χ��(2048+-100),���򱨴�*/
		if(CurrentOffset_t.A_Offset<1948||CurrentOffset_t.A_Offset>2148 ||
		   CurrentOffset_t.B_Offset<1948||CurrentOffset_t.B_Offset>2148 ||
		   CurrentOffset_t.C_Offset<1948||CurrentOffset_t.C_Offset>2148	)
		{
			 TE_ADC_Calibration_t.Errstate=FOC_FAULT;
			 /*״̬��ת*/
			 FOC_State_t=FOC_Error;
		}
		else
		{
			/*״̬��ת*/
			Calib_State=Calib_Encoder;
		}
		
		
		

	}
}

/**
   * @brief  ��Ƕ���ƫУ׼���� 2msִ��һ��
   * @param  ��
   * @retval ��
   */
#ifdef USE_ABZ_ENCODER
void FOC_Task_Encoder_Calibration(void)
{
	static int16_t i=0;
	/*0��Ƕ�����ǿ�� 1��Ƕ��Լ�ǿ�� 2��Ƕ���0 3������תѰ��Z��*/
	static uint8_t run_flag;
	static float mid_angle,end_angle;
	
	/*2msִ��һ��*/
	TE_Encoder_Calibration_t.Cnt_20kHz++;
	
	if(TE_Encoder_Calibration_t.Cnt_20kHz>=40)
	{
		/*����ǿ��*/
		if(run_flag==0)
		{
			if(i<=1000)
			{
				/*��Ƕ���0����������2PI*/
				FOC_Encoder_Calibration_t.theta_el=_2PI*i/1000.0f;
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i++;
			}
			/*�õ�Ƕȹ̶���2PIһ��ʱ��,ʹ�������ȶ�*/
			else if(i<=1250)
			{
				i++;
			}
			else
			{
				/*��¼��Ƕ�Ϊ2PIʱ��ʱ������ֵ*/
				ABZ_Update(&ABZ_t);
				mid_angle=ABZ_GetCalAngle(&ABZ_t);
				i=1000;
				run_flag=1;
			}
		}
		
		/*�Լ�ǿ��*/
		else if(run_flag==1)
		{
			if(i>=0)
			{
				/*��Ƕ���2PI�����Լ���0*/
				FOC_Encoder_Calibration_t.theta_el=_2PI*i/1000.0f;
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i--;
			}
			/*�õ�Ƕȹ̶���0һ��ʱ��,ʹ�������ȶ�*/
			else if(i>=-250)
			{
				i--;
			}
			else
			{
				/*��¼��Ƕ�Ϊ0ʱ��ʱ������ֵ*/
				ABZ_Update(&ABZ_t);
				end_angle=ABZ_GetCalAngle(&ABZ_t);
				
				/*����������ȡ���ڵ�Ƕ�����������������仯����*/
				/*��Ƕ�����,�������Ƕ�Ҳ�������仯,����Ϊ��,��֮Ϊ��*/
				/*����������Ϊ��*/
				if(mid_angle-end_angle>0.1f)
				{
					ABZ_t.sensor_dir=1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI/(mid_angle-end_angle)+0.5f);
				}
				/*����������Ϊ��*/
				else if(mid_angle-end_angle<-0.1f)
				{
					ABZ_t.sensor_dir=-1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI/(end_angle-mid_angle)+0.5f);
				}
				/*������û�������ߵ������û��*/
				else
				{
					TE_Encoder_Calibration_t.Errstate=FOC_FAULT;
					
					/*״̬��ת*/
					FOC_State_t=FOC_Error;
				}
				
				i=0;
				run_flag=2;
			}
		}
		
		/*��ն�ʱ������ֵ,��Ƕ���0*/
		else if(run_flag==2)
		{
			ABZ_TIM->CNT=0;
			
			/*��ձ������ǶȲ��Ȧ��*/
			ABZ_t.d_angle=0.0f;
			ABZ_t.velocity=0.0f;
			ABZ_t.loop=0;
			
			i=0;
			run_flag=3;
		}
		
		/*������תֱ����⵽Z������*/
		else if(run_flag==3)
		{
			Z_use_flag=1;
			/*��ʱ��û��⵽Z������*/
			if(Z_detect_flag==0)
			{
				if(i<=Motor_t.Pole_Pairs*1000)
				{
					FOC_Encoder_Calibration_t.theta_el=_normalizeAngle(_2PI*i/1000.0f);
					I_Park_Transform(&FOC_Encoder_Calibration_t);
					SVPWM_Cal(&FOC_Encoder_Calibration_t);
					SetPWM(&FOC_Encoder_Calibration_t);
					i++;
				}
				/*ת��һȦ��û��⵽Z������,˵������������*/
				else
				{
					i=0;
					/*״̬��ת*/
					FOC_State_t=FOC_Error;
				}
			}
			
			/*��⵽Z������*/
			else if(Z_detect_flag==1)
			{
				ABZ_t.zero_enc_offset=ABZ_TIM->CNT;
				
				i=0;
				run_flag=0;
				Z_use_flag=0;
				Z_detect_flag=0;
				
				/*У׼�����洢*/
				Flash_Save();
					
				/*״̬��ת*/
				Calib_State=Calib_Done;
			}
			

		}
		
		
		TE_Encoder_Calibration_t.Cnt_20kHz=0;
	}
}
#endif

#ifdef USE_SPI_ENCODER
void FOC_Task_Encoder_Calibration(void)
{
	static int16_t i=0;
	static uint8_t run_flag;/*0��Ƕ�����ǿ�� 1��Ƕ��Լ�ǿ�� 2��¼��������ƫ*/
	static float mid_angle,end_angle;
	static uint32_t zero_enc_sum;
	
	/*2msִ��һ��*/
	TE_Encoder_Calibration_t.Cnt_20kHz++;
	
	if(TE_Encoder_Calibration_t.Cnt_20kHz>=40)
	{
		/*����ǿ��*/
		if(run_flag==0)
		{
			if(i<=1000)
			{
				/*��Ƕ���0����������2PI*/
				FOC_Encoder_Calibration_t.theta_el=_2PI*i/1000.0f;
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i++;
			}
			/*�õ�Ƕȹ̶���2PIһ��ʱ��,ʹ�������ȶ�*/
			else if(i<=1250)
			{
				i++;
			}
			else
			{
				/*���µ�ǰ�ı�����ֵ*/
				TLE5012B_Update(&TLE5012B_t);
				/*��¼��Ƕ�Ϊ2PIʱ��������ֵ*/
				mid_angle=TLE5012B_GetCalAngle(&TLE5012B_t);
				i=1000;
				run_flag=1;
			}
		}
		
		/*�Լ�ǿ��*/
		else if(run_flag==1)
		{
			if(i>=0)
			{
				/*��Ƕ���2PI�����Լ���0*/
				FOC_Encoder_Calibration_t.theta_el=_2PI*i/1000.0f;
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i--;
			}
			/*�õ�Ƕȹ̶���0һ��ʱ��,ʹ�������ȶ�*/
			else if(i>=-250)
			{
				i--;
			}
			else
			{
				/*���µ�ǰ�ı�����ֵ*/
				TLE5012B_Update(&TLE5012B_t);
				/*��¼��Ƕ�Ϊ0ʱ��������ֵ*/
				end_angle=TLE5012B_GetCalAngle(&TLE5012B_t);
				
				/*����������ȡ���ڵ�Ƕ�����������������仯����*/
				/*��Ƕ�����,�������Ƕ�Ҳ�������仯,����Ϊ��,��֮Ϊ��*/
				/*����������Ϊ��*/
				if(mid_angle-end_angle>0.1f)
				{
					TLE5012B_t.sensor_dir=1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI/(mid_angle-end_angle)+0.5f);
				}
				/*����������Ϊ��*/
				else if(mid_angle-end_angle<-0.1f)
				{
					TLE5012B_t.sensor_dir=-1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI/(end_angle-mid_angle)+0.5f);
				}
				/*������û�������ߵ������û��*/
				else
				{
					TE_Encoder_Calibration_t.Errstate=FOC_FAULT;
					
					/*״̬��ת*/
					FOC_State_t=FOC_Error;
				}
				
				i=0;
				run_flag=2;
			}
		}
		
		/*��¼��������ƫ*/
		else if(run_flag==2)
		{
			if(i<100)
			{
				zero_enc_sum+=ReadTLE5012B(READ_ANGLE_VALUE);
				i++;
			}
			else
			{
				/*��β���ȡ��ֵ*/
				zero_enc_sum/=100;
				
				/*��¼��������ƫ*/
				TLE5012B_t.zero_enc_offset=TLE5012B_t.resolution-(uint16_t)zero_enc_sum;
				
				/*��ձ������ǶȲ��Ȧ��*/
				TLE5012B_t.d_angle=0.0f;
				TLE5012B_t.velocity=0.0f;
				TLE5012B_t.loop=0;
				
				i=0;
				
				run_flag=0;
				
				/*У׼�����洢*/
				Flash_Save();
				
				/*״̬��ת*/
				//Calib_State=Calib_Anticogging;
				Calib_State=Calib_Done;
			}
		}
		
		TE_Encoder_Calibration_t.Cnt_20kHz=0;
	}
}
#endif

void Calib_Currentloop(void)
{
	/*����������������*/
	if(Current_Cal(&FOC_Anticogging_Calibration_t,&CurrentOffset_t)==FOC_OK)
	{
		/*����������*/
		TLE5012B_Update(&TLE5012B_t);
		
		/*Clarke�任*/
		Clarke_Transform(&FOC_Anticogging_Calibration_t);
			
		/*Park�任*/
		Park_Transform(&FOC_Anticogging_Calibration_t);
			
		/*PID�������*/
		PID_Id_Calib.samp_value=FOC_Anticogging_Calibration_t.Id;
		PID_Iq_Calib.samp_value=FOC_Anticogging_Calibration_t.Iq;
		FOC_Anticogging_Calibration_t.Ud=Current_PI_Ctrl(&PID_Id_Calib);
		FOC_Anticogging_Calibration_t.Uq=Current_PI_Ctrl(&PID_Iq_Calib);
			
		/*��Park�任*/
		I_Park_Transform(&FOC_Anticogging_Calibration_t);
	}
	else
	{
		TE_Calib_Currentloop_t.Errstate=FOC_FAULT;
	}
	
}

void Calib_Speedloop(void)
{
	Encoder_TypeDef *Encoder_t;
	
	Encoder_t=&TLE5012B_t;
	
	/*��������ȡ����*/
	if(Encoder_Cal(&FOC_Anticogging_Calibration_t,Encoder_t,Motor_t.Pole_Pairs)==FOC_OK)
	{
		PID_Speed_Calib.samp_value=Encoder_t->velocity;
		PID_Iq_Calib.ref_value=Speed_PI_Ctrl(&PID_Speed_Calib);
	}
	else
	{
		TE_Calib_Speedloop_t.Errstate=FOC_FAULT;
	}
	
}

/**
   * @brief  ���ݲ�ת��У׼����
   * @param  ��
   * @retval ��
   */
void FOC_Task_Anticogging_Calibration(void)
{
	/*���Ia,Ib,Ic��ƫ��ֵ*/
	float Iph[3]={0.0f,0.0f,0.0f};
	/*������������,0-200*/
	static uint16_t samp_cnt;
	uint16_t enc_val;
	static float run_flag;
	
	if(TE_Anticogging_Calibration_t.Init_Flag==0)
	{
		Flash_Read();
		//Flash_Anticogging_Clear();
		TE_Anticogging_Calibration_t.Init_Flag=1;
	}
	
	/*�ٶȱջ���ʼ������5s,���ٶ��ȶ�*/
	if(run_flag==0)
	{
		if(++TE_Anticogging_Calibration_t.Cnt_20kHz<=100000)
		{	
			/*������ִ��Ƶ��Ϊ10kHz*/
			if(++TE_Calib_Currentloop_t.Cnt_20kHz>=2)
			{
				Calib_Currentloop();
				TE_Calib_Currentloop_t.Cnt_20kHz=0;
			}
			
			/*�ٶȻ�ִ��Ƶ��Ϊ5kHz*/
			if(++TE_Calib_Speedloop_t.Cnt_20kHz>=4)
			{
				Calib_Speedloop();
				TE_Calib_Speedloop_t.Cnt_20kHz=0;
			}
			
			SVPWM_Cal(&FOC_Anticogging_Calibration_t);
			SetPWM(&FOC_Anticogging_Calibration_t);
		}
		else
		{
			TE_Anticogging_Calibration_t.Cnt_20kHz=0;
			run_flag=1;
		}
	}
	
	/*��ʼ��¼����*/
	else if(run_flag==1)
	{
		/*������ִ��Ƶ��Ϊ10kHz*/
		if(++TE_Calib_Currentloop_t.Cnt_20kHz>=2)
		{
			Calib_Currentloop();
			TE_Calib_Currentloop_t.Cnt_20kHz=0;
		}
		
		/*�ٶȻ�ִ��Ƶ��Ϊ5kHz*/
		if(++TE_Calib_Speedloop_t.Cnt_20kHz>=4)
		{
			Calib_Speedloop();
			TE_Calib_Speedloop_t.Cnt_20kHz=0;
		}
		
		SVPWM_Cal(&FOC_Anticogging_Calibration_t);
		SetPWM(&FOC_Anticogging_Calibration_t);
		
		if(++TE_Anticogging_Calibration_t.Cnt_20kHz>=200)
		{
			enc_val=TLE5012B_t.raw_value/16;
//			Iph[0]=FOC_Anticogging_Calibration_t.Ia;
//			Iph[1]=FOC_Anticogging_Calibration_t.Ic;
//			Iph[2]=FOC_Anticogging_Calibration_t.Ib;
//			Flash_Anticog_Save(enc_val,Iph);
			Anticog_1[enc_val]=ADC1->JDR1;
			Anticog_2[enc_val]=ADC1->JDR2;
			Anticog_3[enc_val]=ADC1->JDR3;
			samp_cnt++;
			TE_Anticogging_Calibration_t.Cnt_20kHz=0;
		}
		
		if(samp_cnt>=2000)
		{
			samp_cnt=0;
			
			run_flag=0;
			
			/*״̬��ת*/
			Calib_State=Calib_Done;
		}
	}

}

/**
   * @brief  У׼�������״̬��
   * @param  ��
   * @retval ��
   */
void FOC_Task_Calibration(void)
{
	switch(Calib_State)
	{
		case Calib_ADC:
			FOC_Task_ADC_Calibration();
		break;
		
		case Calib_Encoder:
			FOC_Task_Encoder_Calibration();
		break;
		
		case Calib_Anticogging:
			FOC_Task_Anticogging_Calibration();
		break;
		
		case Calib_Done:
		{	
			Calib_State=Calib_ADC;
			FOC_State_t=FOC_Wait;
		}
		break;
		
		default:break;
	}
	
	
}