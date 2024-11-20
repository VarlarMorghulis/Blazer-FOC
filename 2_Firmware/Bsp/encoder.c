#include "encoder.h"

Encoder_TypeDef Encoder;

extern MotorControl_TypeDef MotorControl;

void Encoder_ParamInit(void)
{
    Encoder.raw                 = 0;
    Encoder.count_in_cpr        = 0;
    Encoder.count_in_cpr_prev   = 0;
    Encoder.shadow_count        = 0;
    Encoder.pos_cpr_counts      = 0;
    Encoder.vel_estimate_counts = 0;
    Encoder.pos                 = 0;
    Encoder.vel                 = 0;
    Encoder.phase               = 0;
    Encoder.phase_vel           = 0;
    Encoder.interpolation       = 0;

    int   encoder_pll_bw   	   = 2000;
    float bandwidth            = fast_min(encoder_pll_bw, 0.25f * PWM_TIM_FREQ);
    Encoder.pll_kp         = 2.0f * bandwidth;           // basic conversion to discrete time
    Encoder.pll_ki         = 0.25f * fast_sq(Encoder.pll_kp); // Critically damped
    Encoder.snap_threshold = 0.5f * Current_Ts * Encoder.pll_ki;
	
	/*编码器SPI模式初始化*/
	switch(Encoder.encoder_type)
	{
		case TLE5012B:
			enc_spi.Init.CLKPolarity = SPI_POLARITY_LOW;
			enc_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
			enc_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
		
			Encoder.resolution=15;
			Encoder.cpr=32768;
		break;
		
		case MT6816:
			enc_spi.Init.CLKPolarity = SPI_POLARITY_HIGH;
			enc_spi.Init.CLKPhase = SPI_PHASE_2EDGE;
			enc_spi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
		
			Encoder.resolution=14;
			Encoder.cpr=16384;
		break;
		
		default:break;
	}
	if (HAL_SPI_Init(&enc_spi) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
   * @brief  读取TLE5012B原始数据
   * @param  reg TLE5012B寄存器地址
   * @retval 返回的角度原始值 0-32767
   */
uint16_t ReadTLE5012B_Raw(uint16_t Reg)
{
	uint16_t ReceiveDat = 0;
	
	ENC_CS_ENABLE;
	HAL_SPI_Transmit(&enc_spi, (uint8_t *)(&Reg), 1, 0x10);
	HAL_SPI_Receive(&enc_spi, (uint8_t *)(&ReceiveDat), 1, 0x10);
	ENC_CS_DISABLE;
	
	return(ReceiveDat & 0x7FFF);
}

/**
   * @brief  读取MT6816原始数据
   * @param  无
   * @retval 返回的角度原始值 0-16383
   */
uint16_t ReadMT6816_Raw(void)
{
	uint16_t sample_data;		
	//uint8_t no_mag_flag;	
	uint8_t pc_flag;	
	uint16_t angle = 0;
	
	uint16_t data_t[2];
	uint16_t data_r[2];
	uint8_t h_count;
	
	data_t[0] = (0x80 | 0x03) << 8;
	data_t[1] = (0x80 | 0x04) << 8;
	
	for(uint8_t i=0; i<3; i++)
	{
		/*读取SPI数据*/
		ENC_CS_ENABLE;
		HAL_SPI_TransmitReceive(&enc_spi, (uint8_t*)&data_t[0], (uint8_t*)&data_r[0], 1, 0xFF);
		ENC_CS_DISABLE;
		ENC_CS_ENABLE;
		HAL_SPI_TransmitReceive(&enc_spi, (uint8_t*)&data_t[1], (uint8_t*)&data_r[1], 1, 0xFF);
		ENC_CS_DISABLE;
		sample_data = ((data_r[0] & 0x00FF) << 8) | (data_r[1] & 0x00FF);
		/*奇偶校验*/
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
		//no_mag_flag = (uint8_t)(sample_data & (0x0001 << 1));
	}

	return angle;
}

uint16_t ReadSPIEncoder_Raw(void)
{
	uint16_t encoder_raw;
	switch(Encoder.encoder_type)
	{
		case TLE5012B:
			encoder_raw = ReadTLE5012B_Raw(READ_ANGLE_VALUE);
		break;
		
		case MT6816:
			encoder_raw = ReadMT6816_Raw();
		break;
		
		default:break;
	}
	return encoder_raw;
}

void Encoder_Update(void)
{
	static int encoderdisconnect_count;
	
	if(MotorControl.encoder_dir == +1)
		Encoder.raw = ReadSPIEncoder_Raw();
	else if(MotorControl.encoder_dir == -1)
		Encoder.raw = Encoder.cpr - ReadSPIEncoder_Raw();
	
	if(Encoder.raw == 0 || Encoder.raw == 1 || Encoder.raw == Encoder.cpr)
	{
		if(++ encoderdisconnect_count >= 200)
		{
			if(MotorControl.ModeNow == Calib_EncoderOffset || 
			   MotorControl.ModeNow == Current_Mode ||
  			   MotorControl.ModeNow == Speed_Mode ||
			   MotorControl.ModeNow == Position_Mode)
			{
				Set_ErrorNow(Encoder_Error);
			}
			encoderdisconnect_count = 0;
		}
	}
	else
	{
		encoderdisconnect_count = 0;
	}
	
    /* Linearization */
	int off_bit     = Encoder.resolution - 7;
    int off_1       = MotorControl.offset_lut[(Encoder.raw) >> off_bit];             // lookup table lower entry
    int off_2       = MotorControl.offset_lut[((Encoder.raw >> off_bit) + 1) % 128]; // lookup table higher entry
    int off_interp  = off_1
                       + ((off_2 - off_1) * (Encoder.raw - ((Encoder.raw >> off_bit) << off_bit))
                        >> off_bit); // Interpolate between lookup table entries
    int count = Encoder.raw - off_interp;
	
    /*  Wrap in ENCODER_CPR */
    while (count > Encoder.cpr)
        count -= Encoder.cpr;
    while (count < 0)
        count += Encoder.cpr;
    Encoder.count_in_cpr = count;
	
    /* Delta count */
    int delta_count           = Encoder.count_in_cpr - Encoder.count_in_cpr_prev;
    Encoder.count_in_cpr_prev = Encoder.count_in_cpr;
    while (delta_count > +(Encoder.cpr >> 1))
        delta_count -= Encoder.cpr;
    while (delta_count < -(Encoder.cpr >> 1))
        delta_count += Encoder.cpr;
	
    /* Add measured delta to encoder count */
    Encoder.shadow_count += delta_count;
	
    /* Run vel PLL */
    Encoder.pos_cpr_counts += Current_Ts * Encoder.vel_estimate_counts;
    float delta_pos_cpr_counts = (float) (Encoder.count_in_cpr - (int) Encoder.pos_cpr_counts);
    while (delta_pos_cpr_counts > +(Encoder.cpr >> 1))
        delta_pos_cpr_counts -= (float)Encoder.cpr;
    while (delta_pos_cpr_counts < -(Encoder.cpr >> 1))
        delta_pos_cpr_counts += (float)Encoder.cpr;
    Encoder.pos_cpr_counts += Current_Ts * Encoder.pll_kp * delta_pos_cpr_counts;
    while (Encoder.pos_cpr_counts > Encoder.cpr)
        Encoder.pos_cpr_counts -= (float)Encoder.cpr;
    while (Encoder.pos_cpr_counts < 0)
        Encoder.pos_cpr_counts += (float)Encoder.cpr;
    Encoder.vel_estimate_counts += Current_Ts * Encoder.pll_ki * delta_pos_cpr_counts;

    // align delta-sigma on zero to prevent jitter
    bool snap_to_zero_vel = false;
    if (fast_abs(Encoder.vel_estimate_counts) < Encoder.snap_threshold) {
        Encoder.vel_estimate_counts = 0.0f;
        snap_to_zero_vel            = true;
    }

    // run encoder count interpolation
    // if we are stopped, make sure we don't randomly drift
    if (snap_to_zero_vel) {
        Encoder.interpolation = 0.5f;
        // reset interpolation if encoder edge comes
    } else if (delta_count > 0) {
        Encoder.interpolation = 0.0f;
    } else if (delta_count < 0) {
        Encoder.interpolation = 1.0f;
    } else {
        // Interpolate (predict) between encoder counts using vel_estimate,
        Encoder.interpolation += Current_Ts * Encoder.vel_estimate_counts;
        // don't allow interpolation indicated position outside of [enc, enc+1)
        if (Encoder.interpolation > 1.0f)
            Encoder.interpolation = 1.0f;
        if (Encoder.interpolation < 0.0f)
            Encoder.interpolation = 0.0f;
    }
    float interpolated_enc = Encoder.count_in_cpr - MotorControl.encoder_offset + Encoder.interpolation;
    while (interpolated_enc > Encoder.cpr)
        interpolated_enc -= Encoder.cpr;
    while (interpolated_enc < 0)
        interpolated_enc += Encoder.cpr;

    float shadow_count_f = (float)Encoder.shadow_count;
    float turns          = shadow_count_f / (float)Encoder.cpr;
    float residual       = shadow_count_f - turns * (float)Encoder.cpr;

    /* Outputs from Encoder for Controller */
    Encoder.pos = turns + residual / (float)Encoder.cpr;
    UTILS_LP_MOVING_AVG_APPROX(Encoder.vel, (Encoder.vel_estimate_counts / (float)Encoder.cpr), 5);
	
	if(MotorControl.motor_pole_pairs <= 0 || MotorControl.motor_pole_pairs > 30)
		Set_ErrorNow(PolePairs_Error);

    Encoder.phase     = (interpolated_enc * _2PI * MotorControl.motor_pole_pairs) / (float)Encoder.cpr;
    Encoder.phase_vel = Encoder.vel * _2PI * MotorControl.motor_pole_pairs;
}

float Encoder_GetEleVel(void)
{
	return Encoder.phase_vel;
}

float Encoder_GetMecVel(void)
{
	return (Encoder.phase_vel / (float)MotorControl.motor_pole_pairs);
}

float Encoder_GetElePhase(void)
{
	return Encoder.phase;
}

float Encoder_GetMecPhase(void)
{
	return Encoder.pos;
}



