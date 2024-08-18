#include "interface_can.h"

SendMsg_TypeDef SendMsg_t;

ReceiveMsg_TypeDef ReceiveMsg_t=
{
	.NodeID=0x01,
	.ParamID=0x00
};

uint8_t CAN_Rxflag=0;

extern FOC_State FOC_State_t;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern FOC_TypeDef FOC_Sensored_t;
extern uint32_t CAN_Rx_timeout;
extern uint8_t Z_confirm_flag;

/**
   * @brief  CAN1滤波器初始化函数
   * @param  无
   * @retval 无
   */
void CAN_Filter_Init(void)
{
	CAN_FilterTypeDef CAN_FilterStruct;
	
	CAN_FilterStruct.FilterActivation=ENABLE;
	CAN_FilterStruct.FilterMode=CAN_FILTERMODE_IDMASK;
	CAN_FilterStruct.FilterScale=CAN_FILTERSCALE_32BIT;
	CAN_FilterStruct.FilterIdHigh=0X0000;
	CAN_FilterStruct.FilterIdLow=0X0000;
	CAN_FilterStruct.FilterMaskIdHigh=0x0000;
	CAN_FilterStruct.FilterMaskIdLow=0x0000;
	CAN_FilterStruct.FilterBank=0;
	CAN_FilterStruct.FilterFIFOAssignment=CAN_RX_FIFO0;
	
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterStruct);
	HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CAN_DataTransform(void)
{
	/*将接收到的-16384 -- +16384转化为-40pi -- +40pi rad/s的设定转速*/
	//PID_Speed.ref_value=((int16_t)(ReceiveMsg_t.given_speed))*20.0f*_2PI/16384.0f;
	/*速度限幅处理*/
	PID_Speed.ref_value=_constrain(PID_Speed.ref_value,-125.663706f,125.663706f);
	
	/*将-40A -- +40A的当前电流转化为-16384 -- +16384的待发送数据*/
	SendMsg_t.real_current=(uint16_t)((int16_t)(FOC_Sensored_t.Iq*40.0f/16384.0f));
	
#ifdef USE_SPI_ENCODER
	/*发送编码器角度原始值*/
	SendMsg_t.enc=SPI_Encoder_t.raw_value;
	/*发送编码器速度由rad/s转为rpm*/
	SendMsg_t.speed_rpm=(uint16_t)((int16_t)(SPI_Encoder_t.velocity*9.549296585f));
#endif

#ifdef USE_ABZ_ENCODER
	/*发送编码器角度原始值*/
	SendMsg_t.enc=ABZ_Enc_t.raw_value;
	/*发送编码器速度由rad/s转为rpm*/
	SendMsg_t.speed_rpm=(uint16_t)((int16_t)(ABZ_Enc_t.velocity*9.549296585f));
#endif

}

//void DataTransform(uint8_t param,)

/**
   * @brief  CAN1数据接收函数
   * @param  无
   * @retval 无
   */
void CANRxIRQHandler(void)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t node_id;
	uint8_t param_id;
	uint8_t rx_data[4];
	int32_t int_data=0;
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rx_data);
	
	/*提取高3位*/
	node_id  = CAN_RxHeaderStruct.StdId >> 8;
	/*提取低8位*/
	param_id = CAN_RxHeaderStruct.StdId & 0x0FF;
	
	/*接收到的节点ID与驱动的节点ID对应*/
	if(node_id == ReceiveMsg_t.NodeID)
	{
		/*将接收超时标志位清零,类似看门狗的实现方式*/
		CAN_Rx_timeout=0;

#ifdef USE_SPI_ENCODER
		if(CAN_Rxflag==0)
		{	/*有感闭环模式*/
			FOC_State_t=FOC_Sensored;
			CAN_Rxflag=1;
		}
		
		int_data |= rx_data[0] << 8*3;
		int_data |= rx_data[1] << 8*2;
		int_data |= rx_data[2] << 8*1;
		int_data |= rx_data[3];
		
		ReceiveMsg_t.data=(float)int_data;
		
		//CAN_DataTransform();
#endif

#ifdef USE_ABZ_ENCODER
	if(Z_confirm_flag==1)
	{
		/*有感闭环模式*/
		FOC_State_t=FOC_Sensored;
		
		ReceiveMsg_t.given_speed=0;
		ReceiveMsg_t.given_speed|=rx_data[0]<<8*1;
		ReceiveMsg_t.given_speed|=rx_data[1];
		
		CAN_DataTransform();
	}
#endif


	LED_G_TOGGLE;
	}
}

/**
   * @brief  CAN1数据发送中断函数
   * @param  无
   * @retval 无
   */
void CAN_SendMessage(void)
{
	CAN_TxHeaderTypeDef CAN_TxHeaderStruct;
	uint8_t send_num;
	uint8_t tx_data[8];
	uint32_t pTxMailbox=0;
	
	CAN_TxHeaderStruct.StdId=ReceiveMsg_t.NodeID;
	CAN_TxHeaderStruct.ExtId=0;
	CAN_TxHeaderStruct.IDE=CAN_ID_STD;
	CAN_TxHeaderStruct.RTR=CAN_RTR_DATA;/*数据帧*/
	CAN_TxHeaderStruct.DLC=0x08;/*8个字节的载荷数据*/
	CAN_TxHeaderStruct.TransmitGlobalTime=ENABLE;
	
	CAN_DataTransform();
	
	/*转子机械角度高8位*/
	tx_data[0]=SendMsg_t.enc>>8;
	/*转子机械角度低8位*/
	tx_data[1]=SendMsg_t.enc;
	/*转子速度高8位*/
	tx_data[2]=SendMsg_t.speed_rpm>>8;
	/*转子速度低8位*/
	tx_data[3]=SendMsg_t.speed_rpm;
	/*实际转矩电流高8位*/
	tx_data[4]=SendMsg_t.real_current>>8;
	/*实际转矩电流低8位*/
	tx_data[5]=SendMsg_t.real_current;
	/*驱动板温度*/
	tx_data[6]=SendMsg_t.temperature;
	/*NULL*/
	tx_data[7]=0x00;
	
	while(HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeaderStruct,tx_data,&pTxMailbox)!=HAL_OK)
	{
		/*发送阻滞*/
		if(++send_num==10)
			break;
	}
}

void CAN_LostConnect_Handle(void)
{
	/*超时则清空电机之前的运行数据,防止重新连接后过冲*/
	PID_Iq.ref_value=0.0f;
	PID_Speed.ref_value=0.0f;
	
	LED_G(0);
	/*将CAN接收标志位置0*/
	CAN_Rxflag=0;
	/*进入闲置状态*/
	FOC_State_t=FOC_Wait;
	
}

void CAN_ID_Conflict_Detect(void)
{
	
}
