#include "foc_calibration.h"

FOC_TypeDef FOC_Encoder_Calibration_t=
{
	.Udc=1.0f,
	.Ud=0.12f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
};

FOC_TypeDef FOC_Encoder_Linearization_t=
{
	.Udc=1.0f,
	.Ud=0.08f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
};

TaskElement_TypeDef TE_ADC_Calibration_t=
{
	.Step=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Encoder_Calibration_t=
{
	.Step=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

TaskElement_TypeDef TE_Encoder_Linearization_t=
{
	.Step=0,
	.Cnt_20kHz=0,
	.Errstate=FOC_OK
};

CurrentOffset_TypeDef CurrentOffset_t=
{
	.A_Offset=2048,
	.B_Offset=2048,
	.C_Offset=2048,
};


Calib_State calib_state=Calib_ADC;

uint8_t Z_use_flag;
uint8_t Z_detect_flag;

extern FOC_State FOC_State_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern Motor_TypeDef Motor_t;
extern InterfaceParam_TypeDef InterfaceParam_t;
extern uint8_t flashsave_flag;

/**
   * @brief  ADC电流采样零偏校准任务 执行频率为20kHz
   * @param  无
   * @retval 无
   */
void FOC_Task_ADC_Calibration(void)
{
	static float A_offset_sum,B_offset_sum,C_offset_sum;
	
	if(++TE_ADC_Calibration_t.Cnt_20kHz<=10000)
	{
		A_offset_sum+=(float)ADC1->JDR1;
		B_offset_sum+=(float)ADC1->JDR2;
		C_offset_sum+=(float)ADC1->JDR3;
	}
	else
	{
		CurrentOffset_t.A_Offset=(uint16_t)(A_offset_sum/10000.0f);
		CurrentOffset_t.B_Offset=(uint16_t)(B_offset_sum/10000.0f);
		CurrentOffset_t.C_Offset=(uint16_t)(C_offset_sum/10000.0f);
		
		A_offset_sum=0;
		B_offset_sum=0;
		C_offset_sum=0;
		
		TE_ADC_Calibration_t.Cnt_20kHz=0;
		
		/*电流采样零偏在一个合理范围内(2048+-100),否则报错*/
		if(CurrentOffset_t.A_Offset<1948||CurrentOffset_t.A_Offset>2148 ||
		   CurrentOffset_t.B_Offset<1948||CurrentOffset_t.B_Offset>2148 ||
		   CurrentOffset_t.C_Offset<1948||CurrentOffset_t.C_Offset>2148	)
		{
			 TE_ADC_Calibration_t.Errstate=FOC_FAULT;
			 /*状态跳转*/
			 FOC_State_t=FOC_Error;
		}
		else
		{
			/*状态跳转*/
			calib_state=Calib_Encoder;
		}
	}
}

/**
   * @brief  编码器校准任务
			 获取编码器零偏和极性 辨识电机极对数
   * @param  无
   * @retval 无
   */
#ifdef USE_ABZ_ENCODER
void FOC_Task_Encoder_Calibration(void)
{
	/*step*/
	/*0电角度自增强拖 1电角度自减强拖 2电角度置0 3开环旋转寻找Z相*/
	static int16_t i=0;
	static float mid_angle,end_angle;
	
	/*2ms执行一次*/
	TE_Encoder_Calibration_t.Cnt_20kHz++;
	
	if(TE_Encoder_Calibration_t.Cnt_20kHz>=40)
	{
		/*自增强拖*/
		if(TE_Encoder_Calibration_t.Step==0)
		{
			if(i<=1000)
			{
				/*电角度由0缓慢自增到2PI*/
				FOC_Encoder_Calibration_t.theta_e=_normalizeAngle(_2PI*2.0f*i/1000.0f);
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i++;
			}
			/*让电角度固定在2PI一段时间,使读数更稳定*/
			else if(i<=1250)
			{
				i++;
			}
			else
			{
				/*记录电角度为2PI时定时器计数值*/
				ABZ_Update(&ABZ_Enc_t);
				mid_angle=ABZ_GetCalAngle(&ABZ_Enc_t);
				i=1000;
				TE_Encoder_Calibration_t.Step=1;
			}
		}
		
		/*自减强拖*/
		else if(TE_Encoder_Calibration_t.Step==1)
		{
			if(i>=0)
			{
				/*电角度由2PI缓慢自减到0*/
				FOC_Encoder_Calibration_t.theta_e=_normalizeAngle(_2PI*2.0f*i/1000.0f);
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i--;
			}
			/*让电角度固定在0一段时间,使读数更稳定*/
			else if(i>=-250)
			{
				i--;
			}
			else
			{
				/*记录电角度为0时定时器计数值*/
				ABZ_Update(&ABZ_Enc_t);
				end_angle=ABZ_GetCalAngle(&ABZ_Enc_t);
				
				/*编码器极性取决于电角度自增方向与编码器变化方向*/
				/*电角度自增,编码器角度也呈自增变化,则极性为正,反之为负*/
				/*编码器极性为正*/
				if(mid_angle-end_angle>0.1f)
				{
					ABZ_Enc_t.sensor_dir=1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI*2.0f/(mid_angle-end_angle)+0.5f);
				}
				/*编码器极性为负*/
				else if(mid_angle-end_angle<-0.1f)
				{
					ABZ_Enc_t.sensor_dir=-1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI*2.0f/(end_angle-mid_angle)+0.5f);
				}
				/*编码器没读数或者电机基本没动*/
				else
				{
					TE_Encoder_Calibration_t.Errstate=FOC_FAULT;
					
					/*状态跳转*/
					FOC_State_t=FOC_Error;
				}
				
				i=0;
				TE_Encoder_Calibration_t.Step=2;
			}
		}
		
		/*清空定时器计数值,电角度置0*/
		else if(TE_Encoder_Calibration_t.Step==2)
		{
			ABZ_TIM->CNT=0;
			
			/*清空编码器角度差和圈数*/
			ABZ_Enc_t.d_angle=0.0f;
			ABZ_Enc_t.velocity=0.0f;
			ABZ_Enc_t.loop=0;
			
			i=0;
			TE_Encoder_Calibration_t.Step=3;
		}
		
		/*开环旋转直到检测到Z相脉冲*/
		else if(TE_Encoder_Calibration_t.Step==3)
		{
			Z_use_flag=1;
			/*暂时还没检测到Z相脉冲*/
			if(Z_detect_flag==0)
			{
				if(i<=Motor_t.Pole_Pairs*1000)
				{
					FOC_Encoder_Calibration_t.theta_e=_normalizeAngle(_2PI*i/1000.0f);
					I_Park_Transform(&FOC_Encoder_Calibration_t);
					SVPWM_Cal(&FOC_Encoder_Calibration_t);
					SetPWM(&FOC_Encoder_Calibration_t);
					i++;
				}
				/*转完一圈还没检测到Z相脉冲,说明接线有问题*/
				else
				{
					i=0;
					/*状态跳转*/
					FOC_State_t=FOC_Error;
				}
			}
			
			/*检测到Z相脉冲*/
			else if(Z_detect_flag==1)
			{
				ABZ_Enc_t.zero_enc_offset=ABZ_TIM->CNT;
				
				i=0;
				TE_Encoder_Calibration_t.Step=0;
				Z_use_flag=0;
				Z_detect_flag=0;
				
				/*校准参数存储*/
				Flash_Save();
					
				/*状态跳转*/
				calib_state=Calib_Done;
			}
		}
		
		TE_Encoder_Calibration_t.Cnt_20kHz=0;
	}
}
#endif


#ifdef USE_SPI_ENCODER
void FOC_Task_Encoder_Calibration(void)
{
	/*step*/
	/*0电角度自增强拖 1电角度自减强拖 2记录编码器零偏*/
	static int i=0;
	static float mid_angle,end_angle;
	static uint32_t zero_enc_sum;
	
	FOC_StructBind(&FOC_Encoder_Calibration_t);
	
	TE_Encoder_Calibration_t.Cnt_20kHz++;
	
	/*4ms执行一次*/
	if(TE_Encoder_Calibration_t.Cnt_20kHz>=80)
	{
		/*自增强拖*/
		if(TE_Encoder_Calibration_t.Step==0)
		{
			if(i<=2000)
			{
				/*电角度由0缓慢自增到2PI*/
				FOC_Encoder_Calibration_t.theta_e=_normalizeAngle(_2PI*2.0f*i/2000.0f);
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i++;
			}
			/*让电角度固定在4PI一段时间,使读数更稳定*/
			else if(i<=2200)
			{
				/*更新当前的编码器值*/
				SPI_Encoder_Update(&SPI_Encoder_t);
				/*记录电角度为2PI时编码器的值*/
				mid_angle+=SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
				i++;
			}
			else
			{
				mid_angle/=200.0f;
				i=2000;
				TE_Encoder_Calibration_t.Step=1;
			}
		}
		
		/*自减强拖*/
		else if(TE_Encoder_Calibration_t.Step==1)
		{
			if(i>=0)
			{
				/*电角度由2PI缓慢自减到0*/
				FOC_Encoder_Calibration_t.theta_e=_normalizeAngle(_2PI*2.0f*i/2000.0f);
				I_Park_Transform(&FOC_Encoder_Calibration_t);
				SVPWM_Cal(&FOC_Encoder_Calibration_t);
				SetPWM(&FOC_Encoder_Calibration_t);
				i--;
			}
			/*让电角度固定在0一段时间,使读数更稳定*/
			else if(i>=-200)
			{
				/*更新当前的编码器值*/
				SPI_Encoder_Update(&SPI_Encoder_t);
				/*记录电角度为0时编码器的值*/
				end_angle+=SPI_Encoder_GetCalAngle(&SPI_Encoder_t);
				i--;
			}
			else
			{
				end_angle/=200.0f;
				
				/*编码器极性取决于电角度自增方向与编码器变化方向*/
				/*电角度自增,编码器角度也呈自增变化,则极性为正,反之为负*/
				/*编码器极性为正*/
				if(mid_angle-end_angle>0.1f)
				{
					SPI_Encoder_t.sensor_dir=1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI*2.0f/(mid_angle-end_angle)+0.5f);
				}
				/*编码器极性为负*/
				else if(mid_angle-end_angle<-0.1f)
				{
					SPI_Encoder_t.sensor_dir=-1;
					Motor_t.Pole_Pairs=(uint8_t)(_2PI*2.0f/(end_angle-mid_angle)+0.5f);
				}
				/*编码器没读数或者电机基本没动*/
				else
				{
					TE_Encoder_Calibration_t.Errstate=FOC_FAULT;
					
					/*状态跳转*/
					FOC_State_t=FOC_Error;
				}
				
				i=0;
				TE_Encoder_Calibration_t.Step=2;
			}
		}
		
		/*记录编码器零偏*/
		else if(TE_Encoder_Calibration_t.Step==2)
		{
			if(i<100)
			{
				zero_enc_sum+=ReadSPIEncoder_Raw();
				i++;
			}
			else
			{
				/*多次测量取均值*/
				zero_enc_sum/=100;
				
				/*记录编码器零偏*/
				SPI_Encoder_t.zero_enc_offset=SPI_Encoder_t.cpr-(uint16_t)zero_enc_sum;
				
				/*清空编码器角度差和圈数*/
				SPI_Encoder_t.d_angle=0.0f;
				SPI_Encoder_t.velocity=0.0f;
				SPI_Encoder_t.loop=0;
				
				i=0;
				TE_Encoder_Calibration_t.Step=0;	
				/*状态跳转*/
				//calib_state=Linearize_Encoder;
				calib_state=Calib_Done;
			}
		}
		
		TE_Encoder_Calibration_t.Cnt_20kHz=0;
	}
}
#endif

extern float angle_ref;

uint16_t enc_temp[256];
uint16_t lut_enc[256];

/**
   * @brief  编码器位置角度线性化任务
			 
   * @param  无
   * @retval 无
   */
void FOC_Task_Encoder_Linearization(void)
{
	const int n=256;
	const int n2=100*Motor_t.Pole_Pairs;
	float delta=_2PI*Motor_t.Pole_Pairs/(n*n2);
	static float theta_ref;
	
	static int i=0;
	static int j=0;
	static int index=0;
	
	static uint8_t step=0;
	
	FOC_StructBind(&FOC_Encoder_Linearization_t);
	
	/*强拖一圈*/
	if(step==0)
	{
		/*一圈内等间距地采集256个点的编码器值*/
		if(i<n)
		{
			if(j<n2)
			{
				theta_ref+=SPI_Encoder_t.sensor_dir*delta;

				FOC_Encoder_Linearization_t.theta_e=_normalizeAngle(theta_ref);
				I_Park_Transform(&FOC_Encoder_Linearization_t);
				SVPWM_Cal(&FOC_Encoder_Linearization_t);
				SetPWM(&FOC_Encoder_Linearization_t);
				
				j++;
			}
			else
			{
				enc_temp[i]=ReadSPIEncoder_Raw();
				j=0;
				i++;
			}
		}
		else
		{
			i=0;
			j=0;
			step=1;
		}
	}	
	
	/*数据处理*/
	else if(step==1)
	{
		/*找到编码器过零点时刻的采样点*/
		while(enc_temp[i]<enc_temp[i+1] && i<n-1)
		{
			i++;
		}
		
		index=i;
		
		/*以零点为分界,编码器值从小到大排序*/
		for(i=0;i<index+1;i++)
		{
			lut_enc[i-index+n]=enc_temp[i];
		}
		for(i=index+1;i<n;i++)
		{
			lut_enc[i-index-1]=enc_temp[i];
		}
		
		/*在最后补一位,后续查表时判断逻辑更加简介*/
		//lut_enc[256]=lut_enc[0]+32767;
		
		/*重置变量*/
		i=0;
		j=0;
		index=0;
		step=0;
		calib_state=Calib_Done;	
	}
}


void FOC_Task_Param_Save(void)
{
	InterfaceParam_t.currentoffset_a=(float)CurrentOffset_t.A_Offset;
	InterfaceParam_t.currentoffset_b=(float)CurrentOffset_t.B_Offset;
	InterfaceParam_t.currentoffset_c=(float)CurrentOffset_t.C_Offset;
	InterfaceParam_t.sensor_dir=(float)SPI_Encoder_t.sensor_dir;
	InterfaceParam_t.zero_enc_offset=(float)SPI_Encoder_t.zero_enc_offset;
	InterfaceParam_t.pole_pairs=(float)Motor_t.Pole_Pairs;
	
	flashsave_flag=1;
	
	calib_state=Calib_ADC;
	FOC_State_t=FOC_Wait;
}

/**
   * @brief  校准任务二级状态机
   * @param  无
   * @retval 无
   */
void FOC_Task_Calibration(void)
{
	switch(calib_state)
	{
		case Calib_ADC:
			FOC_Task_ADC_Calibration();
		break;
		
		case Calib_Encoder:
			FOC_Task_Encoder_Calibration();
		break;
		
		case Linearize_Encoder:
			FOC_Task_Encoder_Linearization();
		break;
		
		case Calib_Done:
			FOC_Task_Param_Save();
		break;
		
		default:break;
	}
	
	
}