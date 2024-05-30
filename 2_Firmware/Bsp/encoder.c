#include "encoder.h"

/*执行频率5000Hz截止频率100Hz*/
/*
IIR_Butterworth_TypeDef Encoder_IIR_LPF_t=
{
	.state_n=0.0f,
	.state_n_1=0.0f,
	.state_n_2=0.0f,
	.a0=1.0f,
	.a1=-1.822695f,
	.a2= 0.837182f,
	.b0=1.0f,
	.b1=2.0f,
	.b2=1.0f,
	.gain0=0.00362168f,
	.gain1=1.0f
};
*/

/*执行频率10000Hz截止频率100Hz*/

IIR_Butterworth_TypeDef Encoder_IIR_LPF_t=
{
	.state_n=0.0f,
	.state_n_1=0.0f,
	.state_n_2=0.0f,
	.a0=1.0f,
	.a1=-1.911197f,
	.a2= 0.914976f,
	.b0=1.0f,
	.b1=2.0f,
	.b2=1.0f,
	.gain0=0.000944692f,
	.gain1=1.0f
};


Encoder_TypeDef TLE5012B_t=
{
	.resolution=32767
};

Encoder_TypeDef ABZ_t=
{
	.resolution=4000
};

extern uint8_t Z_use_flag;
extern uint8_t Z_detect_flag;
uint8_t Z_confirm_flag;
extern FOC_State FOC_State_t;

/**
   * @brief  读取TLE5012B数据
   * @param  reg TLE5012B寄存器地址
   * @retval 读取的原始值
   */
uint16_t ReadTLE5012B(uint16_t Reg)
{
	uint16_t ReceiveDat;
	
	TLE_CS_ENABLE;
	HAL_SPI_Transmit(&ENC_SPI,(uint8_t *)(&Reg),1,0x10);
	HAL_SPI_Receive(&ENC_SPI,(uint8_t *)(&ReceiveDat),1,0x10);
	TLE_CS_DISABLE;
	
	return(ReceiveDat&0x7FFF);
}

/**
   * @brief  TLE5012B更新函数
			 更新当前角度 并根据上一次角度求取速度
   * @param  结构体指针
   * @retval 无                 
   */
void TLE5012B_Update(Encoder_TypeDef *TLE5012B_t)
{
	static uint8_t init_flag=0;
	
	/*读取编码器角度原始值*/
	TLE5012B_t->raw_value=ReadTLE5012B(READ_ANGLE_VALUE);
	/*考虑电角度零偏,并把0-32768转换为0-2pi*/
	TLE5012B_t->angle_now=(TLE5012B_t->raw_value+TLE5012B_t->zero_enc_offset)%32767*0.0001917476f;

	/*当前角度与上一次角度作差*/
	/*由于零偏存在,第一次更新angle_now不为0,但angle_last为0*/
	/*所以初始编码器更新后会计算出一个很大的瞬时速度,第一次需要人为清零*/
	if(init_flag==0)
	{
		TLE5012B_t->d_angle=0.0f;
		init_flag=1;
	}
	else if(init_flag==1)
	{
		TLE5012B_t->d_angle=TLE5012B_t->angle_now-TLE5012B_t->angle_last;
	}
	
	/*正向过零检测,+4.5不是一个定死的值,只要体现出角度过零大幅度突变即可*/
	if(TLE5012B_t->d_angle<-4.5f)
	{
		TLE5012B_t->loop++;
		TLE5012B_t->d_angle+=_2PI;
	}
	/*反向过零检测,-4.5不是一个定死的值,只要体现出角度过零大幅度突变即可*/
	else if(TLE5012B_t->d_angle>4.5f)
	{
		TLE5012B_t->loop--;
		TLE5012B_t->d_angle-=_2PI;
	}

	/*角度差x编码器更新频率算出速度*/
	TLE5012B_t->velocity=TLE5012B_t->sensor_dir * TLE5012B_t->d_angle * 10000.0f;
	
	/*二阶巴特沃斯低通滤波*/
	TLE5012B_t->velocity=IIR_Butterworth_Handle(TLE5012B_t->velocity,&Encoder_IIR_LPF_t);
	
	/*迭代数据*/
	TLE5012B_t->angle_last=TLE5012B_t->angle_now;
}

float TLE5012B_GetRawAngle(Encoder_TypeDef *TLE5012B_t)
{
	return TLE5012B_t->angle_now;
}

float TLE5012B_GetCalAngle(Encoder_TypeDef *TLE5012B_t)
{
	return TLE5012B_t->angle_now+_2PI*(float)TLE5012B_t->loop;
}

float TLE5012B_GetVelocity(Encoder_TypeDef *TLE5012B_t)
{
	return TLE5012B_t->velocity;
}

void ABZ_Resolution_Init(void)
{
	ABZ_TIM->ARR=ABZ_t.resolution-1;
}

/**
   * @brief  ABZ更新函数
			 更新当前角度 并根据上一次角度求取速度
   * @param  结构体指针
   * @retval 无
   */
void ABZ_Update(Encoder_TypeDef *ABZ_t)
{
	/*读取定时器脉冲值*/
	ABZ_t->raw_value=ABZ_TIM->CNT;
	
	/*转化为0-2pi*/
	ABZ_t->angle_now=(float)(ABZ_t->raw_value)/(float)(ABZ_t->resolution-1)*_2PI;
	//ABZ_t->angle_now=(float)(ABZ_t->raw_value)/8191.0f*_2PI;
	
	/*当前角度与上一次角度作差*/
	ABZ_t->d_angle=ABZ_t->angle_now-ABZ_t->angle_last;
	
	/*正向过零检测,+4.5不是一个定死的值,只要体现出角度过零大幅度突变即可*/
	if(ABZ_t->d_angle<-4.5f)
	{
		ABZ_t->loop++;
		ABZ_t->d_angle+=_2PI;
	}
	/*反向过零检测,-4.5不是一个定死的值,只要体现出角度过零大幅度突变即可*/
	else if(ABZ_t->d_angle>4.5f)
	{
		ABZ_t->loop--;
		ABZ_t->d_angle-=_2PI;
	}

	/*角度差x编码器更新频率算出速度*/
	ABZ_t->velocity=ABZ_t->sensor_dir * ABZ_t->d_angle * 10000.0f;
	
	/*二阶巴特沃斯低通滤波*/
	ABZ_t->velocity=IIR_Butterworth_Handle(ABZ_t->velocity,&Encoder_IIR_LPF_t);
	
	/*迭代数据*/
	ABZ_t->angle_last=ABZ_t->angle_now;
	
}

float ABZ_GetCalAngle(Encoder_TypeDef *ABZ_t)
{
	return ABZ_t->angle_now+_2PI*(float)ABZ_t->loop;
}

void ENC_Z_EXTIIRQHandler(void)
{
	if(Z_use_flag==1)
		Z_detect_flag=1;
	else
	{
		if(FOC_State_t==FOC_Wait)
		{
			if(Z_confirm_flag==0)
			{
				Flash_Read();
				LED_G(1);
				Z_confirm_flag=1;
			}
		}
	}
		
}

ErrorState Encoder_Cal(FOC_TypeDef *FOC_t,Encoder_TypeDef *Encoder_t,uint8_t Pole_Pairs)
{
	float angle_el;
	
#ifdef USE_ABZ_ENCODER
	/*编码器更新*/
	ABZ_Update(Encoder_t);
#endif
	
#ifdef USE_SPI_ENCODER
	static uint8_t i=0;
	/*编码器更新*/
	TLE5012B_Update(Encoder_t);
	
	/*连着100次编码器读数都是0x7FFF,说明连接不正常*/
	if(Encoder_t->raw_value==32767)
	{
		i++;
		if(i>=100)
		{
			i=0;
			return FOC_FAULT;
		}
	}
	else
	{
		i=0;
	}
#endif

	/*编码器极性和电机极对数不正常*/
	if(Encoder_t->sensor_dir==0||Pole_Pairs<=1)
	{
		return FOC_FAULT;
	}
	
	else
	{
		/*电角度计算*/
		angle_el=Encoder_t->sensor_dir * Pole_Pairs * Encoder_t->angle_now;
		FOC_t->theta_el=_normalizeAngle(angle_el);
		return FOC_OK;
	}
}
