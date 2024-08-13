#include "interface_uart.h"

Frame_TypeDef Frame_t=
{
	.tail=0x7F800000//结束帧为固定的4个字节
};

extern float theta_ref;
extern float theta_actual;

extern FOC_TypeDef	FOC_Encoder_Calibration_t;
extern FOC_TypeDef FOC_Sensored_t;
extern FOC_TypeDef FOC_Sensorless_t;
extern Motor_TypeDef Motor_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern HFI_TypeDef HFI_t;
extern FOC_TypeDef FOC_HFI_t;
extern FOC_TypeDef FOC_Ls_Identification_t;
extern FOC_TypeDef FOC_Anticogging_Calibration_t;
extern FOC_TypeDef FOC_Openloop_t;
extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern float Motor_Rs;
extern Fluxobserver_TypeDef Fluxobserver_t;
uint16_t indent_i;
extern float Iq_Array[128];
extern float sin_func[20];
void Vofa_Upload(void)
{	
//	Frame_t.fdata[0]=(float)PID_Id.ref_value;
//	Frame_t.fdata[1]=(float)PID_Id.samp_value;
//	Frame_t.fdata[2]=(float)PID_Iq.ref_value;
//	Frame_t.fdata[3]=(float)PID_Iq.samp_value;
//	Frame_t.fdata[4]=(float)PID_Speed.ref_value;
//	Frame_t.fdata[5]=(float)PID_Speed.samp_value;
//	Frame_t.fdata[6]=(float)SPI_Encoder_t.velocity;
//	Frame_t.fdata[7]=(float)FOC_Sensored_t.Ia; 
//	Frame_t.fdata[8]=(float)FOC_Sensored_t.Ib;
//	Frame_t.fdata[9]=(float)FOC_Sensored_t.Ic;
//	Frame_t.fdata[10]=0.0f;
	
//	Frame_t.fdata[0]=(float)Fluxobserver_t.Ialpha;
//	Frame_t.fdata[1]=(float)Fluxobserver_t.Ibeta;
//	Frame_t.fdata[2]=(float)Fluxobserver_t.Ualpha;
//	Frame_t.fdata[3]=(float)Fluxobserver_t.Ubeta;
//	Frame_t.fdata[4]=(float)Fluxobserver_t.cos;
//	Frame_t.fdata[5]=(float)Fluxobserver_t.sin;
//	Frame_t.fdata[6]=(float)FOC_Sensorless_t.theta_el;
//	Frame_t.fdata[7]=(float)FOC_Sensored_t.theta_el;
	
	Frame_t.fdata[0]=FOC_Ls_Identification_t.Iq;
	Frame_t.fdata[1]=FOC_Ls_Identification_t.Uq;
	//Frame_t.fdata[1]=sin_func[indent_i];
	//Frame_t.fdata[1]=arm_sin_f32(_2PI/20.0f*indent_i);
//	indent_i++;
//	if(indent_i>=20)
//		indent_i=0;
	/*高频注入变量查看*/
//	Frame_t.fdata[0]=HFI_t.inj_volt;
//	Frame_t.fdata[1]=FOC_HFI_t.Ia;
//	Frame_t.fdata[2]=FOC_HFI_t.Ib;
//	Frame_t.fdata[3]=FOC_HFI_t.Ic;
//	Frame_t.fdata[4]=FOC_HFI_t.Id;
//	Frame_t.fdata[5]=FOC_HFI_t.Iq;
//	Frame_t.fdata[6]=FOC_HFI_t.Id_h;
//	Frame_t.fdata[7]=FOC_HFI_t.Id_l;
//	Frame_t.fdata[8]=FOC_HFI_t.Iq_h;
//	Frame_t.fdata[9]=FOC_HFI_t.Iq_l;
//	Frame_t.fdata[10]=FOC_HFI_t.theta_el;
//	Frame_t.fdata[11]=0.0f;
	
	/*由于float和uint32_t都为4个字节,因此无需再做结构体数据对齐*/
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)&Frame_t,sizeof(Frame_TypeDef));
}

uint8_t uart_receive;
uint8_t RxState;
uint8_t RxIndex;
uint8_t index_temp;
uint8_t Rx_Flag;
uint8_t Receive_Buf[15];
float speed_rpm;
float weight=1.0f;

void UART_ReceiveEnable(void)
{
	HAL_UART_Receive_IT(&huart1,&uart_receive,1);
}
/**
   * @brief  USART1接收中断回调函数
   * @param  无
   * @retval 无
   */
void UARTRxIRQHandler(void)
{
	LED_R_TOGGLE;

	Receive_Buf[RxIndex]=uart_receive;
	RxIndex++;	
	if(Receive_Buf[RxIndex-2]=='\r' &&
	   Receive_Buf[RxIndex-1]=='\n')
	{
		Rx_Flag=1;
		index_temp=RxIndex;
		RxIndex=0;
	}
	
	if(RxIndex>=15)
		RxIndex=0;
	
	if(Rx_Flag==1)
	{
		if(Receive_Buf[0]=='s'&&
		   Receive_Buf[1]=='p'&&
		   Receive_Buf[2]=='d'	)
		{
			for(uint8_t i=index_temp-3;i>4;i--)
			{
				speed_rpm+=weight*(int8_t)(Receive_Buf[i]-'0');
				weight=weight*10.0f;
			}
			if(Receive_Buf[4]=='-')
			{
				speed_rpm=-speed_rpm;
			}

			PID_Speed.ref_value=speed_rpm/60.0f*_2PI;
			speed_rpm=0.0f;			
			weight=1.0f;
			RxIndex=0;
			for(uint8_t i=0;i<15;i++)
			{
				Receive_Buf[i]=0;
			}
		}
		//memcpy(,)	
		Rx_Flag=0;
    }
	UART_ReceiveEnable();
}


