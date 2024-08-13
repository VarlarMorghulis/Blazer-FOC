#include "foc_run.h"

TaskElement_TypeDef TE_Currentloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Speedloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Positionloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Sensorless_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Openloop_t=
{
	.Init_Flag=0,
	.Run_Flag=0,
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
	.output_max=0.577f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.013f,
	.Ki=121.0f,
	.output_max=0.577f
};
#endif

/*8318*/
#ifdef Motor_8318
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.0006f,
	.Ki=2.5f,
	.output_max=0.577f
};

PID_TypeDef PID_Iq=
{
	.ref_value=5.0f,
	.Kp=0.0006f,
	.Ki=2.5f,
	.output_max=0.577f
};
#endif

/*Tmotor_U10*/
#ifdef Motor_Tmotor_U7
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.00035f,
	.Ki=0.35f,
	.output_max=0.577f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.00035f,
	.Ki=0.35f,
	.output_max=0.577f
};
#endif

/*Tmotor_U10*/
#ifdef Motor_Tmotor_U10
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.0011f,
	.Ki=2.375f,
	.output_max=0.577f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.0011f,
	.Ki=2.375f,
	.output_max=0.577f
};
#endif

PID_TypeDef PID_Speed=
{
	.ref_value=300.0f,
	.Kp=1.5f,
	.Ki=3.0f,
	/*速度环输出最大值就是电流环目标最大值*/
	.output_max=5.0f
};

PID_TypeDef PID_Position=
{
	.ref_value=62.8f,
	.Kp=6.0f,
	.output_max=3.14f
};

extern FOC_State FOC_State_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern Fluxobserver_TypeDef Fluxobserver_t;
extern FOC_TypeDef FOC_Sensorless_t;
extern FOC_TypeDef FOC_HFI_t;

/**
   * @brief  电压速度开环
   * @param  FOC结构体指针 极对数
   * @retval 无
   */
void Open_Voltageloop(FOC_TypeDef *FOC_t,uint8_t Pole_Pairs)
{
	FOC_t->theta_temp+=0.0002f*FOC_t->Speed;
	FOC_t->theta_temp=_normalizeAngle(FOC_t->theta_temp);
	FOC_t->theta_el=_normalizeAngle(FOC_t->theta_temp*Pole_Pairs);
	I_Park_Transform(FOC_t);
	SVPWM_Cal(FOC_t);
	SetPWM(FOC_t);
}

/**
   * @brief  电流速度开环
   * @param  FOC结构体指针 极对数 匀加速到设定速度的时间(s)
   * @retval 无
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
	FOC_t->theta_el=_normalizeAngle(FOC_t->theta_temp*Pole_Pairs);
	
	/*Clarke变换*/
	Clarke_Transform(FOC_t);
		
	/*Park变换*/
	Park_Transform(FOC_t);
		
	/*PID计算输出*/
	PID_Id.samp_value=FOC_t->Id;
	PID_Iq.samp_value=FOC_t->Iq;
	FOC_t->Ud=Current_PI_Ctrl(&PID_Id);
	FOC_t->Uq=Current_PI_Ctrl(&PID_Iq);
		
	/*反Park变换*/
	I_Park_Transform(FOC_t);
	
	SVPWM_Cal(FOC_t);
	
	SetPWM(FOC_t);
}

void FOC_Task_Openloop(void)
{
	/*开环执行频率为10kHz*/
	if(++TE_Openloop_t.Cnt_20kHz>=2)
	{
		//Open_Currentloop(&FOC_Openloop_t,15,0.5f);
		Open_Voltageloop(&FOC_Openloop_t,15);
		TE_Currentloop_t.Cnt_20kHz=0;
	}
}

/**
   * @brief  有感模式电流环函数 20kHz频率
   * @param  无
   * @retval 无
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

	/*编码器更新及角度速度处理*/
	Encoder_Ers=Encoder_Cal(&FOC_Sensored_t,Encoder_t,Motor_t.Pole_Pairs);
	
	/*电流采样及处理*/
	Current_Ers=Current_Cal(&FOC_Sensored_t,&CurrentOffset_t);
	
	/*电流采样运行正常且编码器读取正常*/
	if(Encoder_Ers==FOC_OK && Current_Ers==FOC_OK)
	{
		/*Clarke变换*/
		Clarke_Transform(&FOC_Sensored_t);
		
		/*Park变换*/
		Park_Transform(&FOC_Sensored_t);
		
		/*PID计算输出*/
		PID_Id.samp_value=FOC_Sensored_t.Id;
		PID_Iq.samp_value=FOC_Sensored_t.Iq;
		FOC_Sensored_t.Ud=Current_PI_Ctrl(&PID_Id);
		FOC_Sensored_t.Uq=Current_PI_Ctrl(&PID_Iq);
		
		/*反Park变换*/
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

/**
   * @brief  有感模式速度环函数 10kHz频率
   * @param  无
   * @retval 无
   */
void Sensored_Speedloop(void)
{
	Encoder_TypeDef *Encoder_t;
	
#ifdef USE_ABZ_ENCODER
	Encoder_t=&ABZ_Enc_t;
#endif
	
#ifdef USE_SPI_ENCODER
	Encoder_t=&SPI_Encoder_t;
#endif

//	if(PID_Speed.ref_value<=314.0f)
//	{
//		PID_Speed.ref_value+=0.0628f;
//	}
	PID_Speed.samp_value=Encoder_t->velocity;
	PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
	
}

void Sensored_Positionloop(void)
{
#ifdef USE_ABZ_ENCODER
	//PID_Position.samp_value=ABZ_GetCalAngle(&ABZ_Enc_t);
#endif

#ifdef USE_SPI_ENCODER
	PID_Position.samp_value=SPI_Encoder_t.sensor_dir * SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
#endif
	PID_Speed.ref_value=Position_P_Ctrl(&PID_Position);
}

void FOC_Task_Sensored(void)
{
	/*电流环执行频率为20kHz*/
	if(++TE_Currentloop_t.Cnt_20kHz>=1)
	{
		Sensored_Currentloop();
		TE_Currentloop_t.Cnt_20kHz=0;
	}
	
	/*速度环执行频率为10kHz*/
	if(++TE_Speedloop_t.Cnt_20kHz>=2)
	{
		//Sensored_Speedloop();
		TE_Speedloop_t.Cnt_20kHz=0;
	}
	
	/*位置环执行频率为5kHz*/
	if(++TE_Positionloop_t.Cnt_20kHz>=4)
	{
		//Sensored_Positionloop();
		TE_Positionloop_t.Cnt_20kHz=0;
	}
	
	/*各闭环都正常才会改变占空比*/
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
   * @brief  无感模式电流环函数 20kHz频率
   * @param  无
   * @retval 无
   */

void Sensorless_Currentloop(void)
{
	ErrorState Current_Ers;

	/*电流采样及处理*/
	Current_Ers=Current_Cal(&FOC_Sensorless_t,&CurrentOffset_t);
	/*磁链观测器获取角度*/
	Fluxobserver_Process();
	
	/*电流采样正常*/
	if(Current_Ers==FOC_OK)
	{
		/*Clarke变换*/
		Clarke_Transform(&FOC_Sensorless_t);
		
		/*Park变换*/
		Park_Transform(&FOC_Sensorless_t);
		
		/*PID计算输出*/
		PID_Id.samp_value=FOC_Sensorless_t.Id;
		PID_Iq.samp_value=FOC_Sensorless_t.Iq;
		FOC_Sensorless_t.Ud=Current_PI_Ctrl(&PID_Id);
		FOC_Sensorless_t.Uq=Current_PI_Ctrl(&PID_Iq);
		
		/*反Park变换*/
		I_Park_Transform(&FOC_Sensorless_t);
	}
	else
	{
		if(Current_Ers==FOC_FAULT)
			TE_Currentloop_t.Errstate=FOC_FAULT;	
	}
}

/**
   * @brief  无感模式速度环函数 10kHz频率
   * @param  无
   * @retval 无
   */
void Sensorless_Speedloop(void)
{
	PID_Speed.samp_value=Fluxobserver_t.omega_e/Motor_t.Pole_Pairs;
	PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
}

void FOC_Task_Sensorless(void)
{
	/*电流环执行频率为20kHz*/
	if(++TE_Currentloop_t.Cnt_20kHz>=1)
	{
		Sensorless_Currentloop();
		TE_Currentloop_t.Cnt_20kHz=0;
	}
	
	/*速度环执行频率为10kHz*/
	if(++TE_Speedloop_t.Cnt_20kHz>=2)
	{
		Sensorless_Speedloop();
		TE_Speedloop_t.Cnt_20kHz=0;
	}
	
	/*各闭环都正常才会改变占空比*/
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

