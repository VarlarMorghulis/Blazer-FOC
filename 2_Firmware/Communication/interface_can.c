#include "interface_can.h"

CANMsg_TypeDef CANMsg;

extern MotorControl_TypeDef MotorControl;
extern ModeNow_TypeDef ModeLast;
extern FOC_TypeDef FOC;
extern Encoder_TypeDef Encoder;
extern InterfaceParam_TypeDef InterfaceParam;

/**
   * @brief  CAN1过滤器初始化函数
   * @param  无
   * @retval 无
   */
void CAN1_Filter_Init(void)
{
	CAN_FilterTypeDef CAN_FilterStruct;
	
	/*32位掩码模式*/
	CAN_FilterStruct.FilterActivation = ENABLE;
	CAN_FilterStruct.FilterMode 	  = CAN_FILTERMODE_IDMASK;
	CAN_FilterStruct.FilterScale 	  = CAN_FILTERSCALE_32BIT;
	
	/*筛除节点ID不一致的报文*/
	uint32_t ID   = (uint32_t)((CANMsg.node_id <<  (8+3)) | 0x4);
	uint32_t Mask = (uint32_t)((0x1FFFFF00U << 3) |  0x4  | 0x2);
	CAN_FilterStruct.FilterIdHigh 		= ID   >> 16;
	CAN_FilterStruct.FilterIdLow  		= ID;
	CAN_FilterStruct.FilterMaskIdHigh	= Mask >> 16;
	CAN_FilterStruct.FilterMaskIdLow	= Mask;
	
	CAN_FilterStruct.FilterBank = 0;
	CAN_FilterStruct.FilterFIFOAssignment = CAN_RX_FIFO0;
	
	HAL_CAN_ConfigFilter(&hcan1,&CAN_FilterStruct);
	HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1,CAN_IT_RX_FIFO0_MSG_PENDING);
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
	uint8_t param_id;
	/*u32_data需要赋初值,否则后续接收到的数据可能错乱*/
	uint32_t u32_data = 0;
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &CAN_RxHeaderStruct, CANMsg.rx_data_u8);
	
	/*提取高3位*/
	node_id  = CAN_RxHeaderStruct.ExtId >> 8;
	/*提取低8位*/
	param_id = CAN_RxHeaderStruct.ExtId & 0x0FF;
	
	/*接收到的节点ID与驱动的节点ID对应*/
	if(node_id == CANMsg.node_id)
	{
		CANMsg.can_rx_en = true;
		CANMsg.can_hb_count = 0;
		
		/*恢复连接后清除错误*/
		if(MotorControl.ErrorNow == CAN_DisConnect)
			Set_ErrorNow(No_Error);
		
		u32_data |= CANMsg.rx_data_u8[0] << 24;
		u32_data |= CANMsg.rx_data_u8[1] << 16;
		u32_data |= CANMsg.rx_data_u8[2] << 8;
		u32_data |= CANMsg.rx_data_u8[3];
		
		CANMsg.rx_param_id = (CAN_PARAM_ID)param_id;
		CANMsg.rx_data     = IntBitToFloat(u32_data);
		
		ReceiveMessage_Update(CANMsg.rx_param_id, CANMsg.rx_data);
	}
	
	CAN_RxHeaderStruct.ExtId = 0;
}

void CAN_SendMessage(void)
{
	CAN_TxHeaderTypeDef CAN_TxHeaderStruct;
	uint32_t ID = CANMsg.node_id << 8 | CANMsg.tx_param_id;
	uint8_t send_num = 0;
	uint32_t pTxMailbox = 0;
	
	CAN_TxHeaderStruct.StdId 			  = 0;
	CAN_TxHeaderStruct.ExtId 			  = ID;
	CAN_TxHeaderStruct.IDE   			  = CAN_ID_EXT;
	CAN_TxHeaderStruct.RTR   			  = CAN_RTR_DATA;
	CAN_TxHeaderStruct.DLC				  = 0x04;
	CAN_TxHeaderStruct.TransmitGlobalTime = ENABLE;
	
	while(HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeaderStruct, CANMsg.tx_data_u8, &pTxMailbox) != HAL_OK)
	{
		delay_us(25);
		/*发送阻滞*/
		if(++send_num == 10)
			break;
	}
}

void CAN_DisConnect_Handle(void)
{
	/*心跳设定时间不为0且当前为闭环模式才会启用功能*/
	if(CANMsg.can_hb_set != 0)
	{
		if(MotorControl.ModeNow  == Current_Mode || 
		   MotorControl.ModeNow  == Speed_Mode   || 
		   MotorControl.ModeNow  == Position_Mode  )
		{
			CANMsg.can_hb_en = true;
		}
	}
	else
	{
		CANMsg.can_hb_en = false;
	}
		
	if(CANMsg.can_rx_en == true && CANMsg.can_hb_en == true)
	{
		/*触发超时保护*/
		if(++ CANMsg.can_hb_count >= CANMsg.can_hb_set)
		{
			Set_ErrorNow(CAN_DisConnect);
		}
	}
}

void ModeSwitch_Handle(ModeNow_TypeDef mode_set)
{
	if(MotorControl.ModeNow == Motor_Disable && MotorControl.ErrorNow == No_Error)
	{
		MotorControl.ModeNow = mode_set;
	}
	
	if((MotorControl.ModeNow  == Current_Mode || 
	    MotorControl.ModeNow  == Speed_Mode   || 
	    MotorControl.ModeNow  == Position_Mode) &&
	    MotorControl.ErrorNow == No_Error)
	{
		if(mode_set == Motor_Disable)
			MotorControl.ModeNow = mode_set;
	}
}

/**
   * @brief  接收数据更新
   * @param  param_id 参数ID data 数据
   * @retval 无
   */
void ReceiveMessage_Update(CAN_PARAM_ID param_id, float data)
{
	int data_int = (int)data;
	
	switch(param_id)
	{
		case CAN_SET_MODE:
			if(data_int >=0 && data_int <= 8)
				ModeSwitch_Handle((ModeNow_TypeDef)data_int);
		break;
		case CAN_GET_MODE:
			SendMessage_Update(CAN_GET_MODE, (float)MotorControl.ModeNow);
		break;
		
		case CAN_SET_CURRENT:
			if(ModeLast == Motor_Disable)
				MotorControl.ModeNow = Current_Mode;
			if(data >= -MotorControl.current_limit && data <= MotorControl.current_limit)
				MotorControl.iqRef = data;
		break;
		case CAN_GET_CURRENT_SET:
			SendMessage_Update(CAN_GET_CURRENT_SET, MotorControl.iqRef);
		break;
				
		case CAN_SET_SPEED:
			if(ModeLast == Motor_Disable)
				MotorControl.ModeNow = Speed_Mode;
			if(data >= -MotorControl.speed_limit && data <= MotorControl.speed_limit)
				MotorControl.speedRef = data * _2PI;
		break;
		case CAN_GET_SPEED_SET:
			SendMessage_Update(CAN_GET_SPEED_SET, MotorControl.speedRef / _2PI);
		break;
		
		case CAN_SET_POS:
			if(ModeLast == Motor_Disable)
				MotorControl.ModeNow = Position_Mode;
			MotorControl.posRef  = data * _2PI;
		break;
		case CAN_GET_POS_SET:
			SendMessage_Update(CAN_GET_POS_SET, MotorControl.posRef / _2PI);
		break;
		
		
		
		case CAN_SET_NODE_ID:
			if(data_int >= 0 && data_int <= 7)
				CANMsg.node_id = data;
		break;
		case CAN_GET_NODE_ID:
			SendMessage_Update(CAN_GET_NODE_ID, (float)CANMsg.node_id);
		break;
			
		case CAN_SET_POLEPARIS:
			if(data_int >= 2 && data_int <= 30)
				MotorControl.motor_pole_pairs = data;
		break;
		case CAN_GET_POLEPARIS:
			SendMessage_Update(CAN_GET_POLEPARIS, (float)MotorControl.motor_pole_pairs);
		break;
			
		case CAN_SET_ENCODER_TYPE:
			if(data_int == 0)
				Encoder.encoder_type = TLE5012B;
			else if(data_int == 1)
				Encoder.encoder_type = MT6816;
		break;
		case CAN_GET_ENCODER_TYPE:
			SendMessage_Update(CAN_GET_ENCODER_TYPE, (float)Encoder.encoder_type);
		break;

		case CAN_SET_CURRENT_CAL:
			if(data >= 0.0f && data <= 30.0f)
				MotorControl.calib_current = data;
		break;
		case CAN_GET_CURRENT_CAL:
			SendMessage_Update(CAN_GET_CURRENT_CAL, MotorControl.calib_current);
		break;
		
		case CAN_SET_CURRENT_LIMIT:
			if(data >= 0.0f && data <= 60.0f)
				MotorControl.current_limit = data;
		break;
		case CAN_GET_CURRENT_LIMIT:
			SendMessage_Update(CAN_GET_CURRENT_LIMIT, MotorControl.current_limit);
		break;
		
		case CAN_SET_SPEED_LIMIT:
			if(data >=0.0f && data <= 400.0f)
				MotorControl.speed_limit = data;
		break;
		case CAN_GET_SPEED_LIMIT:
			SendMessage_Update(CAN_GET_SPEED_LIMIT, MotorControl.speed_limit);
		break;
			
		case CAN_SET_ACC:
			if(data >= 0.0f && data <=1000.0f)
				MotorControl.speedAcc = data;
		break;
		case CAN_GET_ACC:
			SendMessage_Update(CAN_GET_ACC, MotorControl.speedAcc);
		break;
		
		case CAN_SET_DEC:
			if(data >= 0.0f && data <= 1000.0f)
				MotorControl.speedDec = data;
		break;
		case CAN_GET_DEC:
			SendMessage_Update(CAN_GET_DEC, MotorControl.speedDec);
		break;
		
		case CAN_SET_SPEED_KP:
			if(data >= 0.01f && data <= 2.0f)
				MotorControl.speed_Kp = data;
		break;
		case CAN_GET_SPEED_KP:
			SendMessage_Update(CAN_GET_SPEED_KP, MotorControl.speed_Kp);
		break;
		
		case CAN_SET_SPEED_KI:
			if(data >= 0.0f && data <= 2.0f)
				MotorControl.speed_Ki = data;
		break;
		case CAN_GET_SPEED_KI:
			SendMessage_Update(CAN_GET_SPEED_KI, MotorControl.speed_Ki);
		break;
		
		case CAN_SET_POS_KP:
			if(data >= 0.01f && data <= 1.0f)
				MotorControl.pos_Kp = data;
		break;
		case CAN_GET_POS_KP:
			SendMessage_Update(CAN_GET_POS_KP, MotorControl.pos_Kp);
		break;

		case CAN_SET_POS_KI:
			if(data >= 0.0f && data <= 1.0f)
				MotorControl.pos_Ki = data;
		break;
		case CAN_GET_POS_KI:
			SendMessage_Update(CAN_GET_POS_KI, MotorControl.pos_Ki);
		break;
		
		case CAN_SET_CAN_HB:
			if((data_int >= 500 && data <= 2000) || data_int == 0)
				CANMsg.can_hb_set = data_int;
		break;
		case CAN_GET_CAN_HB:
			SendMessage_Update(CAN_GET_CAN_HB, (float)CANMsg.can_hb_set);
		break;
		
		case CAN_GET_VBUS:
			SendMessage_Update(CAN_GET_VBUS, MotorControl.vbus);
		break;
		
		
		
		case CAN_GET_IBUS:
			SendMessage_Update(CAN_GET_IBUS, MotorControl.ibus);
		break;
		
		case CAN_GET_IA:
			SendMessage_Update(CAN_GET_IA, MotorControl.ia);
		break;
		
		case CAN_GET_IB:
			SendMessage_Update(CAN_GET_IB, MotorControl.ib);
		break;
		
		case CAN_GET_IC:
			SendMessage_Update(CAN_GET_IC, MotorControl.ic);
		break;
		
		case CAN_GET_ID:
			SendMessage_Update(CAN_GET_ID, MotorControl.id);
		break;
		
		case CAN_GET_IQ:
			SendMessage_Update(CAN_GET_IQ, MotorControl.iq);
		break;
		
		case CAN_GET_SPEED_FILT:
			SendMessage_Update(CAN_GET_SPEED_FILT, Encoder.vel);
		break;
		
		case CAN_GET_ENCODER_RAW:
			SendMessage_Update(CAN_GET_ENCODER_RAW, (float)Encoder.raw);
		break;
		
		case CAN_GET_TEMP:
			SendMessage_Update(CAN_GET_TEMP, (float)MotorControl.temp);
		break;
		
		case CAN_GET_ERROR:
			SendMessage_Update(CAN_GET_ERROR, (float)MotorControl.ErrorNow);
		break;
		
		default:break;
	}
}

void SendMessage_Update(CAN_PARAM_ID param_id, float data)
{
	CANMsg.tx_param_id = param_id;
	CANMsg.tx_dara = data;
	
	uint32_t tx_data_u32;
	
	tx_data_u32 = FloatToIntBit(CANMsg.tx_dara);
	
	CANMsg.tx_data_u8[0] = tx_data_u32 >> 24;
	CANMsg.tx_data_u8[1] = tx_data_u32 >> 16;
	CANMsg.tx_data_u8[2] = tx_data_u32 >> 8;
	CANMsg.tx_data_u8[3] = tx_data_u32;
	
	CANMsg.can_tx_en = true;
}