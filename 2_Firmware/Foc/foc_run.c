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

FOC_TypeDef FOC_Sensored_t=
{
	.Udc=24.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
	.Speed=0.0f
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
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq=
{
	.ref_value=10.0f,
	.Kp=0.013f,
	.Ki=121.0f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};
#endif

/*8318*/
#ifdef Motor_8318
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.012f,
	.Ki=50.0f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.012f,
	.Ki=50.0f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};
#endif

/*韦德*/
#ifdef Motor_Wade
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.035f,
	.Ki=40.0f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.035f,
	.Ki=40.0f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};
#endif

/*Tmotor_U10*/
#ifdef Motor_Tmotor_U10
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.022f,
	.Ki=47.5f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};

PID_TypeDef PID_Iq=
{
	.ref_value=1.0f,
	.Kp=0.022f,
	.Ki=47.5f,
	.error_sum_max=8.0f,
	.output_max=13.0f
};
#endif


/*LD2808*/
/*
PID_TypeDef PID_Id=
{
	.ref_value=0.0f,
	.Kp=0.02f,
	.Ki=155.0f,
	.error_sum_max=8.0f,
	.output_max=13.85f
};

PID_TypeDef PID_Iq=
{
	.ref_value=0.0f,
	.Kp=0.02f,
	.Ki=155.0f,
	.error_sum_max=8.0f,
	.output_max=13.85f
};
*/

PID_TypeDef PID_Speed=
{
	.ref_value=0.0f,
	.Kp=0.3f,
	.Ki=3.0f,
	.error_sum_max=3.0f,
	.output_max=6.0f
};

PID_TypeDef PID_Position=
{
	.ref_value=0.0f,
	.Kp=3.0f,
	.output_max=6.28f
};

extern FOC_State FOC_State_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern Encoder_TypeDef TLE5012B_t;
extern Encoder_TypeDef ABZ_t;
extern FOC_TypeDef FOC_HFI_t;

extern uint16_t Anticog_1[2048];
extern uint16_t Anticog_2[2048];
extern uint16_t Anticog_3[2048];

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
	if(FOC_t->Speed_now<FOC_t->Speed)
	{
		FOC_t->Speed_now+=FOC_t->Speed/runtime*0.0002f;
	}
	FOC_t->theta_temp+=0.0002f*FOC_t->Speed_now;
	FOC_t->theta_temp=_normalizeAngle(FOC_t->theta_temp);
	FOC_t->theta_el=_normalizeAngle(FOC_t->theta_temp*Pole_Pairs);
	I_Park_Transform(FOC_t);
	SVPWM_Cal(FOC_t);
	SetPWM(FOC_t);
}

uint16_t enc_index;
/**
   * @brief  有感模式电流环函数 10kHz频率
   * @param  无
   * @retval 无
   */
void Sensored_Currentloop(void)
{
	Encoder_TypeDef *Encoder_t;
	
#ifdef USE_ABZ_ENCODER
	Encoder_t=&ABZ_t;
#endif
	
#ifdef USE_SPI_ENCODER
	Encoder_t=&TLE5012B_t;
#endif
	
	/*电流采样运行正常且编码器读取正常*/
	if(Current_Cal(&FOC_Sensored_t,&CurrentOffset_t)==FOC_OK &&
	   Encoder_Cal(&FOC_Sensored_t,Encoder_t,Motor_t.Pole_Pairs)==FOC_OK)
	{
		enc_index=TLE5012B_t.raw_value/16;
		
//		FOC_Sensored_t.Ia=(float)Anticog_1[enc_index];
//		FOC_Sensored_t.Ib=(float)Anticog_2[enc_index];
//		FOC_Sensored_t.Ic=(float)Anticog_3[enc_index];
		
//		if(Anticog_1[enc_index]!=0&&Anticog_2[enc_index]!=0&&Anticog_3[enc_index]!=0)
//		{
//			FOC_Sensored_t.Ia-=((float)((int16_t)Anticog_1[enc_index]-(int16_t)CurrentOffset_t.A_Offset))*0.04029304f;
//			FOC_Sensored_t.Ib-=((float)((int16_t)Anticog_2[enc_index]-(int16_t)CurrentOffset_t.B_Offset))*0.04029304f;
//			FOC_Sensored_t.Ic-=((float)((int16_t)Anticog_3[enc_index]-(int16_t)CurrentOffset_t.C_Offset))*0.04029304f;
//		}
		
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
		TE_Currentloop_t.Errstate=FOC_FAULT;
	}
}

/**
   * @brief  有感模式速度环函数 5kHz频率
   * @param  无
   * @retval 无
   */
void Sensored_Speedloop(void)
{
	Encoder_TypeDef *Encoder_t;
	
#ifdef USE_ABZ_ENCODER
	Encoder_t=&ABZ_t;
#endif
	
#ifdef USE_SPI_ENCODER
	Encoder_t=&TLE5012B_t;
#endif

//	if(PID_Speed.ref_value<=314.0f)
//	{
//		PID_Speed.ref_value+=0.0628f;
//	}
	//PID_Speed.samp_value=Encoder_t->velocity;
	//PID_Iq.ref_value=Speed_PI_Ctrl(&PID_Speed);
}

void Sensored_Positionloop(void)
{
#ifdef USE_ABZ_ENCODER
	//PID_Position.samp_value=ABZ_GetCalAngle(&ABZ_t);
#endif

#ifdef USE_SPI_ENCODER
	//PID_Position.samp_value=TLE5012B_GetCalAngle(&TLE5012B_t);
#endif
	//PID_Speed.ref_value=Position_P_Ctrl(&PID_Position);
}

void FOC_Task_Sensored(void)
{
	/*运行前读取Flash存储参数*/
	if(TE_Currentloop_t.Init_Flag==0)
	{
		Flash_Read();
		TE_Currentloop_t.Init_Flag=1;
	}
	
	/*电流环执行频率为10kHz*/
	if(++TE_Currentloop_t.Cnt_20kHz>=2)
	{
		Sensored_Currentloop();
		TE_Currentloop_t.Cnt_20kHz=0;
	}
	
	/*速度环执行频率为5kHz*/
	if(++TE_Speedloop_t.Cnt_20kHz>=4)
	{
		//Open_Voltageloop(&FOC_Sensored_t,2);
		Sensored_Speedloop();
		TE_Speedloop_t.Cnt_20kHz=0;
	}
	
	/*位置环执行频率为1kHz*/
	if(++TE_Positionloop_t.Cnt_20kHz>=20)
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

void FOC_Task_Sensorless(void)
{
	if(++TE_Sensorless_t.Cnt_20kHz>=2)
	{
		HFI_Process();
		TE_Sensorless_t.Cnt_20kHz=0;
	}
	
}
