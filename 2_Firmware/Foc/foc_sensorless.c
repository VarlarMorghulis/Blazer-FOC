#include "foc_sensorless.h"

HFI_TypeDef HFI_t=
{
	.inj_volt_amp=0.5f,
	.inj_volt=0.0f,
	.cnt=0,
	.theta_e=0.0f,
	.w_e=0.0f
};

/*滤波器系数均由MATLAB中FilterDesigner计算得出*/
/*D轴二阶低通滤波器 执行频率为10kHz 截止频率为50Hz*/
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
/*Q轴二阶低通滤波器 执行频率为10kHz 截止频率为50Hz*/
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
/*D轴二阶带通滤波器 执行频率为10kHz 截止频率为470Hz 530Hz*/
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

/*Q轴二阶带通滤波器 执行频率为10kHz 截止频率为470Hz 530Hz*/
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

/*位置误差信号二阶低通滤波器 执行频率为10kHz 截止频率为50Hz*/
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

FOC_TypeDef FOC_HFI_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD
};

PLL_TypeDef PLL_t=
{
	.Kp=0.5f,
	.Ki=5.0f,
	.error_sum_max=6.28f
};

FOC_TypeDef FOC_Sensorless_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD
};

Fluxobserver_TypeDef Fluxobserver_t=
{
	.gamma=100000000.0f,
	.Ts=0.00005f
};

extern FOC_TypeDef FOC_Sensored_t;
extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern AnalogParam_TypeDef AnalogParam_t;

float Rs=0.134f;
float Ls=0.00005987f;
float Flux=0.0030293f;

/**
   * @brief  高频电压生成函数
   * @param  高频注入结构体指针
   * @retval 无
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
   * @brief  锁相环提取角度函数
   * @param  锁相环结构体指针
   * @retval 无
   */
void PLL_Handle(PLL_TypeDef * PLL_t)
{
	PLL_t->error_sum += PLL_t->error * PLL_PERIOD;
	
	PLL_t->output = PLL_t->Kp * PLL_t->error +
					PLL_t->Ki * PLL_t->error_sum;
	
	PLL_t->output_sum += PLL_t->output;
	PLL_t->output_sum=_normalizeAngle(PLL_t->output_sum);
}

void HFI_Process(void)
{
	/*生成高频正弦电压*/
	Generate_HighFrequency_Volt(&HFI_t);
	
	/*计算三相电流*/
	Current_Cal(&FOC_HFI_t,&CurrentOffset_t);
	
	/*Clarke变换*/
	Clarke_Transform(&FOC_HFI_t);
	
	/*Park变换*/
	Park_Transform(&FOC_HFI_t);
	
	/*d轴q轴响应电流低通滤波,提取基波电流*/
	FOC_HFI_t.Id_l=IIR_Butterworth(FOC_HFI_t.Id,&D_IIR_LPF_t);
	FOC_HFI_t.Iq_l=IIR_Butterworth(FOC_HFI_t.Iq,&Q_IIR_LPF_t);
	
	/*d轴q轴响应电流带通滤波,提取高频电流*/
	FOC_HFI_t.Id_h=IIR_Butterworth(FOC_HFI_t.Id,&D_IIR_BPF_t);
	FOC_HFI_t.Iq_h=IIR_Butterworth(FOC_HFI_t.Iq,&Q_IIR_BPF_t);
	
	/*锁相环提取转子角速度和角度*/
	PLL_t.error=IIR_Butterworth(FOC_HFI_t.Iq_h * HFI_t.sin_val,&ERR_IIR_LPF_t);
	PLL_Handle(&PLL_t);
	HFI_t.w_e=PLL_t.output;
	HFI_t.theta_e=PLL_t.output_sum;
	FOC_HFI_t.theta_e=HFI_t.theta_e;
	
	/*电流环PID计算输出*/
	PID_Id.samp_value=FOC_HFI_t.Id_l;
	PID_Iq.samp_value=FOC_HFI_t.Iq_l;
	FOC_HFI_t.Ud=Current_PI_Ctrl(&PID_Id);
	FOC_HFI_t.Uq=Current_PI_Ctrl(&PID_Iq);
	
	/*注入到d轴*/
	FOC_HFI_t.Ud+=HFI_t.inj_volt;
	
	/*反Park变换*/
	I_Park_Transform(&FOC_HFI_t);
	
	SVPWM_Cal(&FOC_HFI_t);
	SetPWM(&FOC_HFI_t);
}



/**
   * @brief  非线性磁链观测器
   * @param  无
   * @retval 无
   */
void Fluxobserver_Process(void)
{
	/*观测器输入变量更新*/
	Fluxobserver_t.Ialpha = FOC_Sensorless_t.Ialpha;
	Fluxobserver_t.Ibeta  = FOC_Sensorless_t.Ibeta;
	Fluxobserver_t.Ualpha = AnalogParam_t.vbus * FOC_Sensorless_t.Ualpha;
	Fluxobserver_t.Ubeta  = AnalogParam_t.vbus * FOC_Sensorless_t.Ubeta;
	
	/*观测器部分*/
	Fluxobserver_t.y1_last=-Rs * Fluxobserver_t.Ialpha + Fluxobserver_t.Ualpha;
	Fluxobserver_t.y2_last=-Rs * Fluxobserver_t.Ibeta  + Fluxobserver_t.Ubeta;
	
	Fluxobserver_t.etax1 = Fluxobserver_t.x1_last - Ls * Fluxobserver_t.Ialpha;
	Fluxobserver_t.etax2 = Fluxobserver_t.x2_last - Ls * Fluxobserver_t.Ibeta;
	
	Fluxobserver_t.phi_err = Flux*Flux-(Fluxobserver_t.etax1 * Fluxobserver_t.etax1 + Fluxobserver_t.etax2 * Fluxobserver_t.etax2);
	
	Fluxobserver_t.x1 = Fluxobserver_t.Ts * (Fluxobserver_t.y1_last + Fluxobserver_t.gamma * Fluxobserver_t.etax1 * Fluxobserver_t.phi_err) + Fluxobserver_t.x1_last;
	Fluxobserver_t.x2 = Fluxobserver_t.Ts * (Fluxobserver_t.y2_last + Fluxobserver_t.gamma * Fluxobserver_t.etax2 * Fluxobserver_t.phi_err) + Fluxobserver_t.x2_last;
	
	/*迭代*/
	Fluxobserver_t.x1_last=Fluxobserver_t.x1;
	Fluxobserver_t.x2_last=Fluxobserver_t.x2;
	
	Fluxobserver_t.cos = (Fluxobserver_t.x1 - Ls * Fluxobserver_t.Ialpha) / Flux;
	Fluxobserver_t.sin = (Fluxobserver_t.x2 - Ls * Fluxobserver_t.Ibeta ) / Flux;
	
	/*反正切提取角度*/
	Fluxobserver_t.theta_e=fast_atan2(Fluxobserver_t.sin,Fluxobserver_t.cos)+_PI;
	
//	if(Fluxobserver_t.theta_e - Fluxobserver_t.theta_temp >4.5f)
//		Fluxobserver_t.omega_e = (Fluxobserver_t.theta_e - Fluxobserver_t.theta_temp - _2PI)*20000.0f;
//	else if(Fluxobserver_t.theta_e - Fluxobserver_t.theta_temp <-4.5f)
//		Fluxobserver_t.omega_e = (Fluxobserver_t.theta_e - Fluxobserver_t.theta_temp + _2PI)*20000.0f;
//	else
//		Fluxobserver_t.omega_e = (Fluxobserver_t.theta_e - Fluxobserver_t.theta_temp)*20000.0f;
//	
//	Fluxobserver_t.theta_temp = Fluxobserver_t.theta_e;
	

}