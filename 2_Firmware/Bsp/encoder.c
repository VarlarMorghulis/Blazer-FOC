#include "encoder.h"

/*ִ��Ƶ��5000Hz��ֹƵ��100Hz*/
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

/*ִ��Ƶ��10000Hz��ֹƵ��100Hz*/

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
	.resolution=8192
};

/**
   * @brief  ��ȡTLE5012B����
   * @param  reg TLE5012B�Ĵ�����ַ
   * @retval ��ȡ��ԭʼֵ
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
   * @brief  TLE5012B���º���
			 ���µ�ǰ�Ƕ� ��������һ�νǶ���ȡ�ٶ�
   * @param  �ṹ��ָ��
   * @retval ��                 
   */
void TLE5012B_Update(Encoder_TypeDef *TLE5012B_t)
{
	/*��ȡ�������Ƕ�ԭʼֵ,�����ǵ�Ƕ���ƫ*/
	TLE5012B_t->raw_value=ReadTLE5012B(READ_ANGLE_VALUE);
	/*��0-32768ת��Ϊ0-2pi*/
	TLE5012B_t->angle_now=(TLE5012B_t->raw_value+TLE5012B_t->zero_enc_offset)%32767*0.0001917476f;

	/*��ǰ�Ƕ�����һ�νǶ�����*/
	TLE5012B_t->d_angle=TLE5012B_t->angle_now-TLE5012B_t->angle_last;
	
	/*���������,+4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	if(TLE5012B_t->d_angle<-4.5f)
	{
		TLE5012B_t->loop++;
		TLE5012B_t->d_angle+=_2PI;
	}
	/*���������,-4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	else if(TLE5012B_t->d_angle>4.5f)
	{
		TLE5012B_t->loop--;
		TLE5012B_t->d_angle-=_2PI;
	}

	/*�ǶȲ�x����������Ƶ������ٶ�*/
	TLE5012B_t->velocity=TLE5012B_t->sensor_dir * TLE5012B_t->d_angle * 10000.0f;
	
	/*���װ�����˹��ͨ�˲�*/
	TLE5012B_t->velocity=IIR_Butterworth_Handle(TLE5012B_t->velocity,&Encoder_IIR_LPF_t);
	
	/*��������*/
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
   * @brief  ABZ���º���
			 ���µ�ǰ�Ƕ� ��������һ�νǶ���ȡ�ٶ�
   * @param  �ṹ��ָ��
   * @retval ��
   */
void ABZ_Update(Encoder_TypeDef *ABZ_t)
{
	/*��ȡ��ʱ������ֵ*/
	ABZ_t->raw_value=ABZ_TIM->CNT;
	
	/*ת��Ϊ0-2pi*/
	ABZ_t->angle_now=(float)(ABZ_t->raw_value)/(float)(ABZ_t->resolution-1)*_2PI;
	//ABZ_t->angle_now=(float)(ABZ_t->raw_value)/8191.0f*_2PI;
	
	/*��ǰ�Ƕ�����һ�νǶ�����*/
	ABZ_t->d_angle=ABZ_t->angle_now-ABZ_t->angle_last;
	
	/*���������,+4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	if(ABZ_t->d_angle<-4.5f)
	{
		ABZ_t->loop++;
		ABZ_t->d_angle+=_2PI;
	}
	/*���������,-4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	else if(ABZ_t->d_angle>4.5f)
	{
		ABZ_t->loop--;
		ABZ_t->d_angle-=_2PI;
	}

	/*�ǶȲ�x����������Ƶ������ٶ�*/
	ABZ_t->velocity=ABZ_t->sensor_dir * ABZ_t->d_angle * 10000.0f;
	
	/*���װ�����˹��ͨ�˲�*/
	ABZ_t->velocity=IIR_Butterworth_Handle(ABZ_t->velocity,&Encoder_IIR_LPF_t);
	
	/*��������*/
	ABZ_t->angle_last=ABZ_t->angle_now;
	
}

float ABZ_GetCalAngle(Encoder_TypeDef *ABZ_t)
{
	return ABZ_t->angle_now+_2PI*(float)ABZ_t->loop;
}

void ENC_Z_EXTIIRQHandler(void)
{
	static uint8_t first_flag=0;
	static uint16_t Offset_CNT=0;
	
	/*�״μ�⵽Z������*/
	if(first_flag==0)
	{
		Offset_CNT=ABZ_TIM->CNT;
		first_flag=1;
	}
	
	/*֮���⵽Z����������״εı���ֵ����*/
	else
	{
		ABZ_TIM->CNT=Offset_CNT;
	}
}

ErrorState Encoder_Cal(FOC_TypeDef *FOC_t,Encoder_TypeDef *Encoder_t,uint8_t Pole_Pairs)
{
	float angle_el;
	
#ifdef USE_ABZ_ENCODER
	/*����������*/
	ABZ_Update(Encoder_t);
#endif
	
#ifdef USE_SPI_ENCODER
	static uint8_t i=0;
	/*����������*/
	TLE5012B_Update(Encoder_t);
	
	/*����100�α�������������0x0000,˵�����Ӳ�����*/
	if(Encoder_t->raw_value==0x0000)
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

	/*���������Ժ͵��������������*/
	if(Encoder_t->sensor_dir==0||Pole_Pairs==0)
	{
		return FOC_FAULT;
	}
	
	else
	{
		/*��Ƕȼ���*/
		angle_el=Encoder_t->sensor_dir * Pole_Pairs * Encoder_t->angle_now;
		FOC_t->theta_el=_normalizeAngle(angle_el);
		return FOC_OK;
	}
}
