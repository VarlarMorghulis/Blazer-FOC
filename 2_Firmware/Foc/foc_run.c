#include "foc_run.h"

TaskElement_TypeDef TE_Currentloop_t=
{
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Speedloop_t=
{
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Positionloop_t=
{
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Sensorless_t=
{
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Openloop_t=
{
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

FOC_TypeDef FOC_Sensored_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
	.Speed=0.0f
};

FOC_TypeDef FOC_Openloop_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.1f,
	.Tpwm=PWM_TIM_PERIOD,
	.Speed=1.0f
};

Motor_TypeDef Motor_t=
{
	.Pole_Pairs=0
};

/*5065*/
#ifdef Motor_5065
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.013f,
	.Ki=121.0f,
	.output_max=0.57735f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.013f,
	.Ki=121.0f,
	.output_max=0.57735f
};
#endif

/*8318*/
#ifdef Motor_8318
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.0006f,
	.Ki=2.5f,
	.output_max=0.57735f
};

PID_TypeDef PID_Iq=
{
	.ref_value=5.0f,
	.Kp=0.0006f,
	.Ki=2.5f,
	.output_max=0.57735f
};
#endif

/*Tmotor_U7*/
#ifdef Motor_Tmotor_U7

PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.000345f,
	.Ki=0.355f,
	.output_max=0.57735f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.000345f,
	.Ki=0.355f,
	.output_max=0.57735f
};


//PID_TypeDef PID_Id=
//{
//	.ref_value=0.0f,
//	.Kp=0.0005987f,
//	.Ki=1.34f,
//	.output_max=0.57735f
//};

//PID_TypeDef PID_Iq=
//{
//	.ref_value=0.0f,
//	.Kp=0.0005987f,
//	.Ki=1.34f,
//	.output_max=0.57735f
//};

/*����*/
/*
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.00003f,
	.Ki=1.4f,
	.output_max=0.57735f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.00003f,
	.Ki=1.4f,
	.output_max=0.57735f
};
*/

#endif

/*Tmotor_U10*/
#ifdef Motor_Tmotor_U10
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.0011f,
	.Ki=2.375f,
	.output_max=0.57735f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.0011f,
	.Ki=2.375f,
	.output_max=0.57735f
};
#endif

#ifdef Motor_Tmotor_U8lite
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.00000765f,
	.Ki=0.03172f,
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.00000765f,
	.Ki=0.03172f,
};

#endif

PID_TypeDef PID_Speed=
{
	.ref_value=0.0f,
	.Kp=0.5f,
	.Ki=0.2f
};

PID_TypeDef PID_ZeroSpeed=
{
	.ref_value=0.0f,
	.Kp=0.5f,
	.Ki=0.2f,
};

PID_TypeDef PID_Position=
{
	.ref_value=0.0f,
	.Kp=0.1f,
	.output_max=100.0f
};

uint8_t sensored_mode=Current_Mode;

extern FOC_State FOC_State_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern Fluxobserver_TypeDef Fluxobserver_t;
extern FOC_TypeDef FOC_Sensorless_t;
extern FOC_TypeDef FOC_HFI_t;

/**
   * @brief  ��ѹ�ٶȿ���
   * @param  FOC�ṹ��ָ�� ������
   * @retval ��
   */
void Open_Voltageloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs)
{
	FOC_t->theta_temp+=0.0002f*FOC_t->Speed;
	FOC_t->theta_temp=_normalizeAngle(FOC_t->theta_temp);
	FOC_t->theta_e=_normalizeAngle(FOC_t->theta_temp*Pole_Pairs);
	I_Park_Transform(FOC_t);
	SVPWM_Cal(FOC_t);
	SetPWM(FOC_t);
}

/**
   * @brief  �����ٶȿ���
   * @param  FOC�ṹ��ָ�� ������ �ȼ��ٵ��趨�ٶȵ�ʱ��(s)
   * @retval ��
   */
void Open_Currentloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs,float runtime)
{
	Current_Cal(FOC_t,&CurrentOffset_t);
	
	if(FOC_t->Speed_now<FOC_t->Speed)
	{
		FOC_t->Speed_now+=FOC_t->Speed/runtime*0.0001f;
	}
	FOC_t->theta_temp+=0.0001f*FOC_t->Speed_now;
	FOC_t->theta_temp=_normalizeAngle(FOC_t->theta_temp);
	FOC_t->theta_e=_normalizeAngle(FOC_t->theta_temp*Pole_Pairs);
	
	/*Clarke�任*/
	Clarke_Transform(FOC_t);
		
	/*Park�任*/
	Park_Transform(FOC_t);
		
	/*PID�������*/
	PID_Id.samp_value=FOC_t->Id;
	PID_Iq.samp_value=FOC_t->Iq;
	FOC_t->Ud=Current_PI_Ctrl(&PID_Id);
	FOC_t->Uq=Current_PI_Ctrl(&PID_Iq);
		
	/*��Park�任*/
	I_Park_Transform(FOC_t);
	
	SVPWM_Cal(FOC_t);
	
	SetPWM(FOC_t);
}

/**
   * @brief  ����ǿ������
   * @param  threshold ǿ�����޸е��ٶȷֽ�ֵ
   * @retval ��
   */
void IF_Start(FOC_TypeDef *FOC_t,float runtime,float threshold)
{
	ErrorState Current_Ers;
	
	Current_Ers=Current_Cal(FOC_t,&CurrentOffset_t);
	
	if(FOC_t->Speed_now<threshold)
	{
		FOC_t->Speed_now+=threshold/runtime*0.00005f;
	}
	FOC_t->theta_temp+=0.00005f*FOC_t->Speed_now;
	FOC_t->theta_temp=_normalizeAngle(FOC_t->theta_temp);
	FOC_t->theta_e=_normalizeAngle(FOC_t->theta_temp);
	
	/*������������*/
	if(Current_Ers==FOC_OK)
	{
		/*Clarke�任*/
		Clarke_Transform(FOC_t);
			
		/*Park�任*/
		Park_Transform(FOC_t);
			
		/*PID�������*/
		PID_Id.samp_value=FOC_t->Id;
		PID_Iq.samp_value=FOC_t->Iq;
		FOC_t->Ud=Current_PI_Ctrl(&PID_Id);
		FOC_t->Uq=Current_PI_Ctrl(&PID_Iq);
	
		/*��ѹԲ����*/
		Circle_Limitation(&FOC_t->Ud,&FOC_t->Uq);
		
		/*��Park�任*/
		I_Park_Transform(FOC_t);
	}
	else
	{
		if(Current_Ers==FOC_FAULT)
			TE_Currentloop_t.Errstate=FOC_FAULT;	
	}
}

void FOC_Task_Openloop(void)
{
	/*����ִ��Ƶ��Ϊ10kHz*/
	if(++TE_Openloop_t.Cnt_20kHz>=2)
	{
		//Open_Currentloop(&FOC_Openloop_t,15,0.5f);
		Open_Voltageloop(&FOC_Openloop_t,15);
		TE_Currentloop_t.Cnt_20kHz=0;
	}
}

/**
   * @brief  �и�ģʽ���������� 20kHzƵ��
   * @param  ��
   * @retval ��
   */
void Sensored_Currentloop(void)
{
	ErrorState Encoder_Ers,Current_Ers;
	Encoder_TypeDef *Encoder_t;
	
#ifdef USE_ABZ_ENCODER
	Encoder_t=&ABZ_Enc_t;
#endif
	
#ifdef USE_SPI_ENCODER
	Encoder_t=&SPI_Encoder_t;
#endif

	/*���������¼��Ƕ��ٶȴ���*/
	Encoder_Ers=Encoder_Cal(&FOC_Sensored_t,Encoder_t,Motor_t.Pole_Pairs);
	
	/*��������������*/
	Current_Ers=Current_Cal(&FOC_Sensored_t,&CurrentOffset_t);
	
	/*�����������������ұ�������ȡ����*/
	if(Encoder_Ers==FOC_OK && Current_Ers==FOC_OK)
	{
		/*Clarke�任*/
		Clarke_Transform(&FOC_Sensored_t);
		
		/*Park�任*/
		Park_Transform(&FOC_Sensored_t);
		
		/*PID�������*/
		PID_Id.samp_value=FOC_Sensored_t.Id;
		PID_Iq.samp_value=FOC_Sensored_t.Iq;
		FOC_Sensored_t.Ud=Current_PI_Ctrl(&PID_Id);
		FOC_Sensored_t.Uq=Current_PI_Ctrl(&PID_Iq);
		
		/*��ѹԲ�޷�*/
		Circle_Limitation(&FOC_Sensored_t.Ud,&FOC_Sensored_t.Uq);
		
		/*��Park�任*/
		I_Park_Transform(&FOC_Sensored_t);
	}
	else
	{
		if(Encoder_Ers==FOC_FAULT)
			TE_Speedloop_t.Errstate=FOC_FAULT;	
		if(Current_Ers==FOC_FAULT)
			TE_Currentloop_t.Errstate=FOC_FAULT;		
	}
}

uint8_t speed_to_pos;
uint8_t spdloop_state=Speed_Mode;
uint8_t spdloop_laststate=Speed_Mode;

void Speedloop_StateReset(void)
{
	speed_to_pos=0;
	spdloop_state=Speed_Mode;
	spdloop_laststate=Speed_Mode;
}

/**
   * @brief  �и�ģʽ�ٶȻ����� 10kHzƵ��
   * @param  ��
   * @retval ��
   */
void Sensored_Speedloop(void)
{
	static uint8_t position_cnt;
	
	/*����λ��ģʽ,����ת�ر���*/
	if(PID_Speed.ref_value==0)
	{	
		if(fast_abs(SPI_Encoder_t.velocity)<= _2PI / 10.0f)
		{
			spdloop_state=Position_Mode;
			
			Clear_PID_Param(&PID_Speed);
			if(spdloop_laststate==Speed_Mode)
			{
				PID_Position.ref_value=SPI_Encoder_t.sensor_dir * SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
				speed_to_pos=1;
			}
		}
		
		if(speed_to_pos==1)
		{
			if(++position_cnt>=2)
			{	
				PID_Position.samp_value = SPI_Encoder_t.sensor_dir * SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
				PID_ZeroSpeed.ref_value=Position_P_Ctrl(&PID_Position);
				position_cnt=0;
			}
			PID_ZeroSpeed.samp_value=SPI_Encoder_t.velocity;
			PID_Iq.ref_value=Speed_PI_Ctrl(&PID_ZeroSpeed);
		}
		else if(speed_to_pos==0)
		{
			PID_Speed.samp_value=SPI_Encoder_t.velocity;
			PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
		}
	}
	else
	{
		spdloop_state=Speed_Mode;
		speed_to_pos=0;
		
		Clear_PID_Param(&PID_ZeroSpeed);
		
		PID_Speed.samp_value=SPI_Encoder_t.velocity;
		PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
	}
	
	spdloop_laststate=spdloop_state;
}

void Sensored_Positionloop(void)
{
	PID_Position.samp_value=SPI_Encoder_t.sensor_dir * SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
	PID_Speed.ref_value=Position_P_Ctrl(&PID_Position);
}

void FOC_Task_Sensored(void)
{
	FOC_StructBind(&FOC_Sensored_t);
	
	switch(sensored_mode)
	{
		case Current_Mode:
			/*������ִ��Ƶ��Ϊ20kHz*/
			if(++TE_Currentloop_t.Cnt_20kHz>=1)
			{
				Sensored_Currentloop();
				TE_Currentloop_t.Cnt_20kHz=0;
			}
		break;
		
		case Speed_Mode:
			/*������ִ��Ƶ��Ϊ20kHz*/
			if(++TE_Currentloop_t.Cnt_20kHz>=1)
			{
				Sensored_Currentloop();
				TE_Currentloop_t.Cnt_20kHz=0;
			}
			
			/*�ٶȻ�ִ��Ƶ��Ϊ10kHz*/
			if(++TE_Speedloop_t.Cnt_20kHz>=2)
			{
				Sensored_Speedloop();
				TE_Speedloop_t.Cnt_20kHz=0;
			}
		break;
		
		case Position_Mode:
			/*������ִ��Ƶ��Ϊ20kHz*/
			if(++TE_Currentloop_t.Cnt_20kHz>=1)
			{
				Sensored_Currentloop();
				TE_Currentloop_t.Cnt_20kHz=0;
			}
			
			/*�ٶȻ�ִ��Ƶ��Ϊ10kHz*/
			if(++TE_Speedloop_t.Cnt_20kHz>=2)
			{
				Sensored_Speedloop();
				TE_Speedloop_t.Cnt_20kHz=0;
			}
			
			/*λ�û�ִ��Ƶ��Ϊ5kHz*/
			if(++TE_Positionloop_t.Cnt_20kHz>=4)
			{
				Sensored_Positionloop();
				TE_Positionloop_t.Cnt_20kHz=0;
			}
		break;
		
		default:break;
	}

	/*���ջ��������Ż�ı�ռ�ձ�*/
	if(TE_Currentloop_t.Errstate == FOC_OK &&
	   TE_Speedloop_t.Errstate   == FOC_OK)
	{
		SVPWM_Cal(&FOC_Sensored_t);
		SetPWM(&FOC_Sensored_t);
	}
	else
	{
		FOC_State_t=FOC_Error;
	}
}

/**
   * @brief  �޸�ģʽ���������� 20kHzƵ��
   * @param  ��
   * @retval ��
   */

void Sensorless_Currentloop(void)
{
	ErrorState Current_Ers;

	/*��������������*/
	Current_Ers=Current_Cal(&FOC_Sensorless_t,&CurrentOffset_t);
	/*�����۲�����ȡ�Ƕ�*/
	Fluxobserver_Process();
	
	/*���Ƕȹ�һ����0-2pi*/
	FOC_Sensorless_t.theta_e=_normalizeAngle(Fluxobserver_t.theta_e);
	/*������������*/
	if(Current_Ers==FOC_OK)
	{
		/*Clarke�任*/
		Clarke_Transform(&FOC_Sensorless_t);
		
		/*Park�任*/
		Park_Transform(&FOC_Sensorless_t);
		
		/*PID�������*/
		PID_Id.samp_value=FOC_Sensorless_t.Id;
		PID_Iq.samp_value=FOC_Sensorless_t.Iq;
		FOC_Sensorless_t.Ud=Current_PI_Ctrl(&PID_Id);
		FOC_Sensorless_t.Uq=Current_PI_Ctrl(&PID_Iq);
		
		/*��ѹԲ����*/
		Circle_Limitation(&FOC_Sensorless_t.Ud,&FOC_Sensorless_t.Uq);
		
		/*��Park�任*/
		I_Park_Transform(&FOC_Sensorless_t);
	}
	else
	{
		if(Current_Ers==FOC_FAULT)
			TE_Currentloop_t.Errstate=FOC_FAULT;	
	}
}

/**
   * @brief  �޸�ģʽ�ٶȻ����� 10kHzƵ��
   * @param  ��
   * @retval ��
   */
void Sensorless_Speedloop(void)
{
	PID_Speed.samp_value=Fluxobserver_t.omega_e/Motor_t.Pole_Pairs;
	PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
}

uint8_t switch_flag;
float speed;
void FOC_Task_Sensorless(void)
{
	FOC_StructBind(&FOC_Sensorless_t);
	
	/*������ִ��Ƶ��Ϊ20kHz*/
	if(++TE_Currentloop_t.Cnt_20kHz>=1)
	{
		Sensorless_Currentloop();
		TE_Currentloop_t.Cnt_20kHz=0;
	}
	
	/*�ٶȻ�ִ��Ƶ��Ϊ10kHz*/
	if(++TE_Speedloop_t.Cnt_20kHz>=2)
	{
		//Sensorless_Speedloop();
		TE_Speedloop_t.Cnt_20kHz=0;
	}
	
	/*���ջ��������Ż�ı�ռ�ձ�*/
	if(TE_Currentloop_t.Errstate == FOC_OK &&
	   TE_Speedloop_t.Errstate   == FOC_OK)
	{
		SVPWM_Cal(&FOC_Sensorless_t);
		SetPWM(&FOC_Sensorless_t);
		
	}
	else
	{
		FOC_State_t=FOC_Error;
	}
}

