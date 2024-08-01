#include "encoder.h"

enum 
{	
	TLE5012B,
	MT6816
};

uint8_t SPI_Encoder_Type=MT6816;

/*ִ��Ƶ��20000Hz��ֹƵ��100Hz*/
IIR_Butterworth_TypeDef Encoder_IIR_LPF_t=
{
	.state_n=0.0f,
	.state_n_1=0.0f,
	.state_n_2=0.0f,
	.a0=1.0f,
	.a1=-1.955578f,
	.a2= 0.956544f,
	.b0=1.0f,
	.b1=2.0f,
	.b2=1.0f,
	.gain0=0.000241359f,
	.gain1=1.0f
};

Encoder_TypeDef SPI_Encoder_t;
Encoder_TypeDef ABZ_Enc_t;

extern uint8_t Z_use_flag;
extern uint8_t Z_detect_flag;
uint8_t Z_confirm_flag;
extern FOC_State FOC_State_t;
extern uint16_t lut_enc[257];

void Encoder_ParamInit(void)
{
	/*������SPIģʽ��ʼ��*/
	switch(SPI_Encoder_Type)
	{
		case TLE5012B:
			enc_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
			enc_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
			enc_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		
			SPI_Encoder_t.resolution=15;
			SPI_Encoder_t.cpr=32768;
		break;
		
		case MT6816:
			enc_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
			enc_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
			enc_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		
			SPI_Encoder_t.resolution=14;
			SPI_Encoder_t.cpr=16384;
		break;
		
		default:break;
	}
	if (HAL_SPI_Init(&enc_spi) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
   * @brief  ��ȡTLE5012Bԭʼ����
   * @param  reg TLE5012B�Ĵ�����ַ
   * @retval ���صĽǶ�ԭʼֵ 0-32767
   */
uint16_t ReadTLE5012B_Raw(uint16_t Reg)
{
	uint16_t ReceiveDat;
	
	ENC_CS_ENABLE;
	HAL_SPI_Transmit(&enc_spi,(uint8_t *)(&Reg),1,0x10);
	HAL_SPI_Receive(&enc_spi,(uint8_t *)(&ReceiveDat),1,0x10);
	ENC_CS_DISABLE;
	
	return(ReceiveDat&0x7FFF);
}

/**
   * @brief  ��ȡMT6816ԭʼ����
   * @param  ��
   * @retval ���صĽǶ�ԭʼֵ 0-16383
   */
uint16_t ReadMT6816_Raw(void)
{
	uint16_t sample_data;		
	uint8_t no_mag_flag;	
	uint8_t pc_flag;	
	uint16_t angle;
	
	uint16_t data_t[2];
	uint16_t data_r[2];
	uint8_t h_count;
	
	data_t[0] = (0x80 | 0x03) << 8;
	data_t[1] = (0x80 | 0x04) << 8;
	
	for(uint8_t i=0; i<3; i++)
	{
		/*��ȡSPI����*/
		ENC_CS_ENABLE;
		HAL_SPI_TransmitReceive(&enc_spi, (uint8_t*)&data_t[0], (uint8_t*)&data_r[0], 1, 0xFF);
		ENC_CS_DISABLE;
		ENC_CS_ENABLE;
		HAL_SPI_TransmitReceive(&enc_spi, (uint8_t*)&data_t[1], (uint8_t*)&data_r[1], 1, 0xFF);
		ENC_CS_DISABLE;
		sample_data = ((data_r[0] & 0x00FF) << 8) | (data_r[1] & 0x00FF);
		/*��żУ��*/
		h_count = 0;
		for(uint8_t j=0; j<16; j++)
		{
			if(sample_data & (0x0001 << j))
				h_count++;
		}
		if(h_count & 0x01){
			pc_flag = 0;
		}
		else{
			pc_flag = 1;
			break;
		}
	}
	if(pc_flag)
	{
		angle = sample_data >> 2;
		no_mag_flag = (uint8_t)(sample_data & (0x0001 << 1));
	}

	return angle;
}

uint16_t ReadSPIEncoder_Raw(void)
{
	uint16_t encoder_raw;
	switch(SPI_Encoder_Type)
	{
		case TLE5012B:
			encoder_raw=ReadTLE5012B_Raw(READ_ANGLE_VALUE);
		break;
		
		case MT6816:
			encoder_raw=ReadMT6816_Raw();
		break;
		
		default:break;
	}
	return encoder_raw;
}

void SPI_Encoder_Update(Encoder_TypeDef *SPI_Encoder_t)
{
	/*��ȡ�������Ƕ�ԭʼֵ*/

	SPI_Encoder_t->raw_value=ReadSPIEncoder_Raw();
	
	SPI_Encoder_t->angle_now=
		( (SPI_Encoder_t->raw_value + SPI_Encoder_t->zero_enc_offset)% SPI_Encoder_t->cpr ) * _2PI/(float)SPI_Encoder_t->cpr;
	
	if(SPI_Encoder_t->closeloop_flag==0)
	{
		/*������ƫ����,��һ�θ���angle_now��Ϊ0,��angle_lastΪ0*/
		/*���Գ�ʼ���������º������һ���ܴ��˲ʱ�ٶ�,��һ����Ҫ��Ϊ����*/
		SPI_Encoder_t->d_angle=0.0f;
		SPI_Encoder_t->closeloop_flag=1;
	}
	else if(SPI_Encoder_t->closeloop_flag==1)
	{
		/*��ǰ�Ƕ�����һ�νǶ�����*/
		SPI_Encoder_t->d_angle=SPI_Encoder_t->angle_now - SPI_Encoder_t->angle_last;
	}
	
	/*���������,+4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	if(SPI_Encoder_t->d_angle<-4.5f)
	{
		SPI_Encoder_t->loop++;
		SPI_Encoder_t->d_angle+=_2PI;
	}
	/*���������,-4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	else if(SPI_Encoder_t->d_angle>4.5f)
	{
		SPI_Encoder_t->loop--;
		SPI_Encoder_t->d_angle-=_2PI;
	}

	/*�ǶȲ�x����������Ƶ������ٶ�*/
	SPI_Encoder_t->velocity=SPI_Encoder_t->sensor_dir * SPI_Encoder_t->d_angle * 20000.0f;
	
	/*���װ�����˹��ͨ�˲�*/
	SPI_Encoder_t->velocity=IIR_Butterworth_Handle(SPI_Encoder_t->velocity,&Encoder_IIR_LPF_t);
	
	/*��������*/
	SPI_Encoder_t->angle_last=SPI_Encoder_t->angle_now;
}

float SPI_Encoder_GetRawAngle(Encoder_TypeDef *SPI_Encoder_t)
{
	return SPI_Encoder_t->angle_now;
}

float SPI_Encoder_GetCalAngle(Encoder_TypeDef *SPI_Encoder_t)
{
	return SPI_Encoder_t->angle_now+_2PI*(float)SPI_Encoder_t->loop;
}

float SPI_Encoder_GetVelocity(Encoder_TypeDef *SPI_Encoder_t)
{
	return SPI_Encoder_t->velocity;
}

void ABZ_Resolution_Init(void)
{
	ABZ_Enc_t.cpr=4000;
	ABZ_TIM->ARR=ABZ_Enc_t.cpr-1;
}

/**
   * @brief  ABZ���º���
			 ���µ�ǰ�Ƕ� ��������һ�νǶ���ȡ�ٶ�
   * @param  �ṹ��ָ��
   * @retval ��
   */
void ABZ_Update(Encoder_TypeDef *ABZ_Enc_t)
{
	/*��ȡ��ʱ������ֵ*/
	ABZ_Enc_t->raw_value=ABZ_TIM->CNT;
	
	/*ת��Ϊ0-2pi*/
	ABZ_Enc_t->angle_now=(float)(ABZ_Enc_t->raw_value)/(float)(ABZ_Enc_t->cpr-1)*_2PI;
	//ABZ_Enc_t->angle_now=(float)(ABZ_Enc_t->raw_value)/8191.0f*_2PI;
	
	/*��ǰ�Ƕ�����һ�νǶ�����*/
	ABZ_Enc_t->d_angle=ABZ_Enc_t->angle_now-ABZ_Enc_t->angle_last;
	
	/*���������,+4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	if(ABZ_Enc_t->d_angle<-4.5f)
	{
		ABZ_Enc_t->loop++;
		ABZ_Enc_t->d_angle+=_2PI;
	}
	/*���������,-4.5����һ��������ֵ,ֻҪ���ֳ��Ƕȹ�������ͻ�伴��*/
	else if(ABZ_Enc_t->d_angle>4.5f)
	{
		ABZ_Enc_t->loop--;
		ABZ_Enc_t->d_angle-=_2PI;
	}

	/*�ǶȲ�x����������Ƶ������ٶ�*/
	ABZ_Enc_t->velocity=ABZ_Enc_t->sensor_dir * ABZ_Enc_t->d_angle * 10000.0f;
	
	/*���װ�����˹��ͨ�˲�*/
	ABZ_Enc_t->velocity=IIR_Butterworth_Handle(ABZ_Enc_t->velocity,&Encoder_IIR_LPF_t);
	
	/*��������*/
	ABZ_Enc_t->angle_last=ABZ_Enc_t->angle_now;
	
}

float ABZ_GetCalAngle(Encoder_TypeDef *ABZ_Enc_t)
{
	return ABZ_Enc_t->angle_now+_2PI*(float)ABZ_Enc_t->loop;
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
	/*����������*/
	ABZ_Update(Encoder_t);
#endif
	
#ifdef USE_SPI_ENCODER
	static int i=0;
	/*����������*/
	SPI_Encoder_Update(Encoder_t);
	
	/*����1000�α����������쳣,˵��Ӳ�����Ӳ�����*/
	if(Encoder_t->raw_value==0 || Encoder_t->raw_value==Encoder_t->cpr)
	{
		i++;
		if(i>=1000)
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
	if(Encoder_t->sensor_dir==0||Pole_Pairs<=1)
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
