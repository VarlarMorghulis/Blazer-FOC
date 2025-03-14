#include "foc_sensorless.h"

HFI_TypeDef HFI_t=
{
	.inj_volt_amp=0.5f,
	.inj_volt=0.0f,
	.cnt=0,
	.theta_e=0.0f,
	.w_e=0.0f
};

/*�˲���ϵ������MATLAB��FilterDesigner����ó�*/
/*D����׵�ͨ�˲��� ִ��Ƶ��Ϊ10kHz ��ֹƵ��Ϊ50Hz*/
IIR_Butterworth_TypeDef D_IIR_LPF_t=
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
	.gain0=0.000241f,
	.gain1=1.0f
};
/*Q����׵�ͨ�˲��� ִ��Ƶ��Ϊ10kHz ��ֹƵ��Ϊ50Hz*/
IIR_Butterworth_TypeDef Q_IIR_LPF_t=
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
	.gain0=0.000241f,
	.gain1=1.0f
};
/*D����״�ͨ�˲��� ִ��Ƶ��Ϊ10kHz ��ֹƵ��Ϊ470Hz 530Hz*/
IIR_Butterworth_TypeDef D_IIR_BPF_t=
{
	.state_n=0.0f,
	.state_n_1=0.0f,
	.state_n_2=0.0f,
	.a0=1.0f,
	.a1=-1.288378f,
	.a2= 0.962994f,
	.b0=1.0f,
	.b1=0.0f,
	.b2=-1.0f,
	.gain0=0.018503f,
	.gain1=1.0f
};

/*Q����״�ͨ�˲��� ִ��Ƶ��Ϊ10kHz ��ֹƵ��Ϊ470Hz 530Hz*/
IIR_Butterworth_TypeDef Q_IIR_BPF_t=
{
	.state_n=0.0f,
	.state_n_1=0.0f,
	.state_n_2=0.0f,
	.a0=1.0f,
	.a1=-1.288378f,
	.a2= 0.962994f,
	.b0=1.0f,
	.b1=0.0f,
	.b2=-1.0f,
	.gain0=0.018503f,
	.gain1=1.0f
};

/*λ������źŶ��׵�ͨ�˲��� ִ��Ƶ��Ϊ10kHz ��ֹƵ��Ϊ50Hz*/
IIR_Butterworth_TypeDef ERR_IIR_LPF_t=
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
	.gain0=0.000241f,
	.gain1=1.0f
};

FOC_TypeDef FOC_HFI_t;

PLL_TypeDef PLL_t=
{
	.Kp=0.5f,
	.Ki=5.0f,
	.error_sum_max=6.28f
};

Fluxobserver_TypeDef Fluxobserver=
{
	.gamma = 1000000000.0f,
	.Ts = 0.00005f
};

extern MotorControl_TypeDef MotorControl;
extern CurrentOffset_TypeDef CurrentOffset;
extern FOC_TypeDef FOC;


/**
   * @brief  ��Ƶ��ѹ���ɺ���
   * @param  ��Ƶע��ṹ��ָ��
   * @retval ��
   */
void Generate_HighFrequency_Volt(HFI_TypeDef * HFI_t)
{
	float angle;
	
	if(++HFI_t->cnt>=HFI_CNT_FLAG)
	{
		HFI_t->cnt=0;
	}
	angle=HFI_t->cnt/((float)HFI_CNT_FLAG)*_2PI;
	HFI_t->inj_volt=HFI_t->inj_volt_amp * arm_cos_f32(angle);
	HFI_t->sin_val=arm_sin_f32(angle);
}

/**
   * @brief  ���໷��ȡ�ǶȺ���
   * @param  ���໷�ṹ��ָ��
   * @retval ��
   */
void PLL_Handle(PLL_TypeDef * PLL_t)
{
	PLL_t->error_sum += PLL_t->error * PLL_PERIOD;
	
	PLL_t->output = PLL_t->Kp * PLL_t->error +
					PLL_t->Ki * PLL_t->error_sum;
	
	PLL_t->output_sum += PLL_t->output;
	PLL_t->output_sum=_normalizeAngle(PLL_t->output_sum);
}

//void HFI_Process(void)
//{
//	/*���ɸ�Ƶ���ҵ�ѹ*/
//	Generate_HighFrequency_Volt(&HFI_t);
//	
//	FOC_HFI_t.Ia = MotorControl.ia;
//	FOC_HFI_t.Ib = MotorControl.ib;
//	FOC_HFI_t.Ic = MotorControl.ic;
//	
//	/*Clarke�任*/
//	//Clarke_Transform(&FOC_HFI_t);
//	
//	/*Park�任*/
//	//Park_Transform(&FOC_HFI_t);
//	
//	/*d��q����Ӧ������ͨ�˲�,��ȡ��������*/
//	FOC_HFI_t.Id_l=IIR_Butterworth(FOC_HFI_t.Id,&D_IIR_LPF_t);
//	FOC_HFI_t.Iq_l=IIR_Butterworth(FOC_HFI_t.Iq,&Q_IIR_LPF_t);
//	
//	/*d��q����Ӧ������ͨ�˲�,��ȡ��Ƶ����*/
//	FOC_HFI_t.Id_h=IIR_Butterworth(FOC_HFI_t.Id,&D_IIR_BPF_t);
//	FOC_HFI_t.Iq_h=IIR_Butterworth(FOC_HFI_t.Iq,&Q_IIR_BPF_t);
//	
//	/*���໷��ȡת�ӽ��ٶȺͽǶ�*/
//	PLL_t.error=IIR_Butterworth(FOC_HFI_t.Iq_h * HFI_t.sin_val,&ERR_IIR_LPF_t);
//	PLL_Handle(&PLL_t);
//	HFI_t.w_e=PLL_t.output;
//	HFI_t.theta_e=PLL_t.output_sum;
//	FOC_HFI_t.theta_e=HFI_t.theta_e;
//	
//	/*������PID�������*/
//	PID_Id.samp_value=FOC_HFI_t.Id_l;
//	PID_Iq.samp_value=FOC_HFI_t.Iq_l;
//	FOC_HFI_t.Ud=Current_PI_Ctrl(&PID_Id);
//	FOC_HFI_t.Uq=Current_PI_Ctrl(&PID_Iq);
//	
//	/*ע�뵽d��*/
//	FOC_HFI_t.Ud+=HFI_t.inj_volt;
//	
//	/*��Park�任*/
//	//I_Park_Transform(&FOC_HFI_t);
//	
//	SVPWM_Cal(&FOC_HFI_t);
//	SetPWM(&FOC_HFI_t);
//}



/**
   * @brief  �����Դ����۲���
   * @param  ��
   * @retval ��
   */
void Fluxobserver_Update(void)
{
	float mod_to_V = FOC.Vbus_filt / 1.5f;
	float Rs   = MotorControl.motor_phase_resistance;
	float Ls   = MotorControl.motor_phase_inductance;
	float flux = MotorControl.motor_flux;
	float delta_theta = 0.0f;
	
	/*�۲��������������*/
	Fluxobserver.Ialpha = FOC.Ialpha;
	Fluxobserver.Ibeta  = FOC.Ibeta;
	Fluxobserver.Ualpha = FOC.mod_alpha * mod_to_V;
	Fluxobserver.Ubeta  = FOC.mod_beta  * mod_to_V;
	
	/*�۲�������*/
	Fluxobserver.y1_last = -Rs * Fluxobserver.Ialpha + Fluxobserver.Ualpha;
	Fluxobserver.y2_last = -Rs * Fluxobserver.Ibeta  + Fluxobserver.Ubeta;
	
	Fluxobserver.etax1 = Fluxobserver.x1_last - Ls * Fluxobserver.Ialpha;
	Fluxobserver.etax2 = Fluxobserver.x2_last - Ls * Fluxobserver.Ibeta;
	
	Fluxobserver.phi_err = fast_sq(flux) - (fast_sq(Fluxobserver.etax1) + fast_sq(Fluxobserver.etax2));
	
	Fluxobserver.x1 = Current_Ts * (Fluxobserver.y1_last + Fluxobserver.gamma * Fluxobserver.etax1 * Fluxobserver.phi_err) + Fluxobserver.x1_last;
	Fluxobserver.x2 = Current_Ts * (Fluxobserver.y2_last + Fluxobserver.gamma * Fluxobserver.etax2 * Fluxobserver.phi_err) + Fluxobserver.x2_last;
	
	/*����*/
	Fluxobserver.x1_last = Fluxobserver.x1;
	Fluxobserver.x2_last = Fluxobserver.x2;
	
	Fluxobserver.cos = (Fluxobserver.x1 - Ls * Fluxobserver.Ialpha) / flux;
	Fluxobserver.sin = (Fluxobserver.x2 - Ls * Fluxobserver.Ibeta ) / flux;
	
	/*��������ȡ�Ƕ�*/
	Fluxobserver.theta_e = fast_atan2(Fluxobserver.sin,Fluxobserver.cos) + _PI;
	
	delta_theta = Fluxobserver.theta_e - Fluxobserver.theta_last;
	
	Fluxobserver.theta_last = Fluxobserver.theta_e;
	
	if(delta_theta < -4.0f)
		delta_theta += _2PI;
	else if(delta_theta > 4.0f)
		delta_theta -= _2PI;
	
	UTILS_LP_FAST(Fluxobserver.omega_e, delta_theta / Current_Ts, 0.1f);
}

float Observer_GetElePhase(void)
{
	return Fluxobserver.theta_e;
}

float Observer_GetEleVel(void)
{
	return Fluxobserver.omega_e;
}