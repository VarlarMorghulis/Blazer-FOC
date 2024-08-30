#include "interface_can.h"

ReceiveMsg_TypeDef ReceiveMsg_t=
{
	.NodeID=0x00,
	.ParamID=0x00
};

uint8_t CAN_Rxflag=0;
uint8_t start_en=0;

extern FOC_State FOC_State_t;
extern PID_TypeDef PID_Id;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_Position;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern Motor_TypeDef Motor_t;
extern FOC_TypeDef FOC_Sensored_t;
extern uint32_t CAN_Rx_timeout;
extern uint8_t Z_confirm_flag;
extern uint8_t sensored_mode;
extern ReceiveMsg_TypeDef ReceiveMsg_t;
extern InterfaceParam_TypeDef InterfaceParam_t;
extern uint8_t flashsave_flag;

/**
   * @brief  CAN1过滤器初始化函数
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

/**
   * @brief  CAN参数处理
   * @param  param_id 参数ID data 数据
   * @retval 无
   */
void CAN_Param_Handle(uint8_t param_id,float data)
{
	switch(param_id)
	{
		case CAN_SET_START_ENABLE:
			if(data==0)
			{
				start_en=1;
				sensored_mode=Current_Mode;
			}
			else if(data==1.0f)
			{
				start_en=1;
				sensored_mode=Speed_Mode;
			}
			else if(data==2.0f)
			{
				start_en=1;
				sensored_mode=Position_Mode;
			}
		break;
		
		case CAN_SET_CURRENT:
			PID_Iq.ref_value = data;
		break;
		
		case CAN_SET_MRPM:
			PID_Speed.ref_value = data / 60.0f * _2PI;
		break;
		
		case CAN_SET_POS:
			PID_Position.ref_value = data * _2PI;
		break;
		
		case CAN_SET_NODE_ID:
			if(data>=0 && data<=7)
			{
				InterfaceParam_t.node_id=data;
				flashsave_flag=1;
			}
		break;
		
		case CAN_SET_POLEPARIS:
			if(data>=2 && data<=40)
			{
				InterfaceParam_t.pole_pairs=data;
				flashsave_flag=1;
			}
		break;
		
		case CAN_SET_ENCODER_TYPE:
			if(data==0)
			{
				InterfaceParam_t.enc_type=(float)TLE5012B;
				flashsave_flag=1;
			}
			else if(data==1.0f)
			{
				InterfaceParam_t.enc_type=(float)MT6816;
				flashsave_flag=1;
			}
		break;
		
		case CAN_SET_MAX_CURRENT:
			if(data>=0 && data<=50)
			{
				InterfaceParam_t.current_max=data;
				flashsave_flag=1;
			}
		break;
		
		case CAN_SET_MAX_MRPM:
			
		break;
		
		case CAN_CALIB:
			if(data==0)
			{
				if(start_en==0)
					FOC_State_t=FOC_Calibration;
			}
		break;
		default:break;
	}
}

/**
   * @brief  CAN1数据接收函数
   * @param  无
   * @retval 无
   */
void CANRxIRQHandler(void)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t node_id;
	uint8_t param_id=0x08;
	uint8_t rx_data[4];
	uint32_t u32_data=0;
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rx_data);
	
	/*提取高3位*/
	node_id  = CAN_RxHeaderStruct.ExtId >> 8;
	/*提取低8位*/
	param_id = CAN_RxHeaderStruct.ExtId & 0x0FF;
	
	/*接收到的节点ID与驱动的节点ID对应*/
	if(node_id == ReceiveMsg_t.NodeID)
	{
		/*心跳标志位置零*/
		CAN_Rx_timeout=0;
		
		if(CAN_Rxflag==0&&start_en==1&&FOC_State_t==FOC_Wait)
		{	
			/*有感闭环模式*/
			FOC_State_t=FOC_Sensored;
			CAN_Rxflag=1;
		}
		
		u32_data |= rx_data[0] << 24;
		u32_data |= rx_data[1] << 16;
		u32_data |= rx_data[2] << 8;
		u32_data |= rx_data[3];
		
		ReceiveMsg_t.ParamID=param_id;
		ReceiveMsg_t.data=IntBitToFloat(u32_data);
		
		CAN_Param_Handle(ReceiveMsg_t.ParamID,ReceiveMsg_t.data);
		
		LED_G_TOGGLE;
	}
	CAN_RxHeaderStruct.ExtId=0;
}

void CAN_LostConnect_Handle(void)
{
	/*超时则清空电机之前的运行数据,防止重新连接后过冲*/
	Clear_PID_Param(&PID_Id);
	Clear_PID_Param(&PID_Iq);
	Clear_PID_Param(&PID_Speed);
	Clear_PID_Param(&PID_Position);
	
	LED_G(0);
	/*将CAN接收标志位置0*/
	CAN_Rxflag=0;
	/*进入闲置状态*/
	FOC_State_t=FOC_Wait;
}

void CAN_ID_Conflict_Detect(void)
{
	
}
