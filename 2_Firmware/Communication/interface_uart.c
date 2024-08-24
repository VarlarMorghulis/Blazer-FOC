#include "interface_uart.h"

Frame_TypeDef Frame_t=
{
	.tail=0x7F800000//结束帧为固定的4个字节
};

extern FOC_TypeDef *FOC_Param_t;
extern Motor_TypeDef Motor_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern HFI_TypeDef HFI_t;
extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_Position;
extern Fluxobserver_TypeDef Fluxobserver_t;

void Vofa_Upload(void)
{	
	Frame_t.fdata[0]=FOC_Param_t->Ia;
	Frame_t.fdata[1]=FOC_Param_t->Ib;
	Frame_t.fdata[2]=FOC_Param_t->Ic;
	Frame_t.fdata[3]=FOC_Param_t->Id;
	Frame_t.fdata[4]=FOC_Param_t->Iq;
	Frame_t.fdata[5]=FOC_Param_t->theta_e;
	Frame_t.fdata[6]=PID_Speed.ref_value;
	Frame_t.fdata[7]=SPI_Encoder_t.velocity;
	Frame_t.fdata[8]=PID_Position.ref_value;
	Frame_t.fdata[9]=PID_Position.samp_value;
	
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


