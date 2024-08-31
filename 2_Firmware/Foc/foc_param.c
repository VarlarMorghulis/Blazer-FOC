#include "foc_param.h"

InterfaceParam_TypeDef InterfaceParam_t;

extern ReceiveMsg_TypeDef ReceiveMsg_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern Motor_TypeDef Motor_t;
extern CurrentOffset_TypeDef CurrentOffset_t;
extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_Position;

void Param_Return_Default(void)
{
	InterfaceParam_t.node_id=(float)0x00;
	InterfaceParam_t.enc_type=(float)TLE5012B;
	//InterfaceParam_t.sensor_dir=(float)1;
	//InterfaceParam_t.zero_enc_offset=(float)SPI_Encoder_t.zero_enc_offset;
	//InterfaceParam_t.currentoffset_a=(float)CurrentOffset_t.A_Offset;
	//InterfaceParam_t.currentoffset_b=(float)CurrentOffset_t.B_Offset;
	//InterfaceParam_t.currentoffset_c=(float)CurrentOffset_t.C_Offset;
	InterfaceParam_t.pole_pairs=7.0f;
	InterfaceParam_t.current_max=40.0f;
	InterfaceParam_t.speed_max=8000.0f / 60.0f * _2PI;
}

void FOC_Param_Init(void)
{
	ReceiveMsg_t.NodeID=(uint8_t)InterfaceParam_t.node_id;
	SPI_Encoder_t.enc_type=(Encoder_Type)InterfaceParam_t.enc_type;
	SPI_Encoder_t.sensor_dir=(int8_t)InterfaceParam_t.sensor_dir;
	SPI_Encoder_t.zero_enc_offset=(uint16_t)InterfaceParam_t.zero_enc_offset;
	CurrentOffset_t.A_Offset=(uint16_t)InterfaceParam_t.currentoffset_a;
	CurrentOffset_t.B_Offset=(uint16_t)InterfaceParam_t.currentoffset_b;
	CurrentOffset_t.C_Offset=(uint16_t)InterfaceParam_t.currentoffset_c;
	Motor_t.Pole_Pairs=(uint8_t)InterfaceParam_t.pole_pairs;
	PID_Speed.output_max=InterfaceParam_t.current_max;
	PID_Position.output_max=InterfaceParam_t.speed_max;
}

FOC_TypeDef *FOC_Param_t;
FOC_TypeDef FOC_Idle_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.0f,
	.Tpwm=PWM_TIM_PERIOD,
	.Tcmp1=4200,.Tcmp2=4200,.Tcmp3=4200
};

/**
   * @brief  将当前运行的FOC结构体指针指向FOC_Param_t 
   * @param  FOC结构体指针
   * @retval 无
   */
void FOC_StructBind(FOC_TypeDef *FOC_t)
{
	FOC_Param_t=FOC_t;
}

/**
   * @brief  将空闲FOC结构体指针指向FOC_Param_t 
   * @param  无
   * @retval 无
   */
void FOC_StructUnbind(void)
{
	FOC_Param_t=&FOC_Idle_t;
}

