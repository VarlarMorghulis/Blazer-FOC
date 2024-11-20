#include "interface_can.h"

float_to_u8 blazer_content_transform[Motor_Num];

CAN_Data_TypeDef CAN_Data[Motor_Num]=
{
	{0x00, (uint8_t*)(&blazer_content_transform[0].u8_data)},
	{0x01, (uint8_t*)(&blazer_content_transform[1].u8_data)},
	{0x02, (uint8_t*)(&blazer_content_transform[2].u8_data)},
	{0x03, (uint8_t*)(&blazer_content_transform[3].u8_data)},
};

uint32_t FloatToIntBit(float x)
{
	uint32_t *pInt;
	pInt=(uint32_t*)(&x);
	return *pInt;
}

float IntBitToFloat(uint32_t x)
{
	float * pFloat;
	pFloat=(float *)(&x);
	return *pFloat;
}

/**
   * @brief  CAN1��������ʼ������
   * @param  ��
   * @retval ��
   */
void CAN_Filter_Init(void)
{
	CAN_FilterTypeDef CAN_FilterStruct;
	
	CAN_FilterStruct.FilterActivation	  = ENABLE;
	CAN_FilterStruct.FilterMode			  = CAN_FILTERMODE_IDMASK;
	CAN_FilterStruct.FilterScale		  = CAN_FILTERSCALE_32BIT;
	CAN_FilterStruct.FilterIdHigh		  = 0x0000;
	CAN_FilterStruct.FilterIdLow		  = 0x0000;
	CAN_FilterStruct.FilterMaskIdHigh	  = 0x0000;
	CAN_FilterStruct.FilterMaskIdLow	  = 0x0000;
	CAN_FilterStruct.FilterBank			  = 0;
	CAN_FilterStruct.FilterFIFOAssignment = CAN_RX_FIFO0;
	
	HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterStruct);
	HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/**
   * @brief  CAN���ͱ���
   * @param  ID �ɽڵ�ID�Ͳ���ID���ɵĸ���ID
   * @retval ��
   */
void CAN_SendMessage(uint32_t ID)
{
	CAN_TxHeaderTypeDef CAN_TxHeaderStruct;
	uint8_t send_num = 0;
	uint32_t pTxMailbox = 0;
	
	CAN_TxHeaderStruct.StdId			  = 0;
	CAN_TxHeaderStruct.ExtId			  = ID;
	CAN_TxHeaderStruct.IDE				  = CAN_ID_EXT;
	CAN_TxHeaderStruct.RTR				  = CAN_RTR_DATA;
	CAN_TxHeaderStruct.DLC				  = 0x04;
	CAN_TxHeaderStruct.TransmitGlobalTime = ENABLE;
	
	while(HAL_CAN_AddTxMessage(&hcan1, &CAN_TxHeaderStruct, CAN_Data[ID >> 8].data_ptr, &pTxMailbox) != HAL_OK)
	{
		delay_us(25);
		/*��������*/
		if(++send_num == 10)
			break;
	}
}

float f_data;
uint8_t node_id;
uint8_t param_id;
void CANRxIRQHandler(void)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;

	uint8_t rx_data[4];
	uint32_t u32_data = 0;
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0, &CAN_RxHeaderStruct, rx_data);
	
	/*��ȡ��3λ*/
	node_id  = CAN_RxHeaderStruct.ExtId >> 8;
	/*��ȡ��8λ*/
	param_id = CAN_RxHeaderStruct.ExtId & 0x0FF;
	
	u32_data |= rx_data[0] << 24;
	u32_data |= rx_data[1] << 16;
	u32_data |= rx_data[2] << 8;
	u32_data |= rx_data[3];
	
	f_data = IntBitToFloat(u32_data);
}


/**
   * @brief  ��ȡ�ڵ�ID
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_node_id(uint32_t node_id)
{
	uint32_t blazer_node_id_id = node_id << 8 | CAN_GET_NODE_ID;

	CAN_SendMessage(blazer_node_id_id);
}

/**
   * @brief  ��ȡ������
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_pole_pairs(uint32_t node_id)
{
	uint32_t blazer_pp_id = node_id << 8 | CAN_GET_POLEPARIS;

	CAN_SendMessage(blazer_pp_id);
}

/**
   * @brief  ��ȡ�������ͺ�
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_encoder_type(uint32_t node_id)
{
	uint32_t blazer_enc_type_id = node_id << 8 | CAN_GET_ENCODER_TYPE;

	CAN_SendMessage(blazer_enc_type_id);
}

/**
   * @brief  ��ȡУ׼����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_current_cal(uint32_t node_id)
{
	uint32_t blazer_current_cal_id = node_id << 8 | CAN_GET_CURRENT_CAL;

	CAN_SendMessage(blazer_current_cal_id);
}

/**
   * @brief  ��ȡ����������
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_current_limit(uint32_t node_id)
{
	uint32_t blazer_current_limit_id = node_id << 8 | CAN_GET_CURRENT_LIMIT;

	CAN_SendMessage(blazer_current_limit_id);
}

/**
   * @brief  ��ȡ���ת������
   * @param  node_ id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_limit(uint32_t node_id)
{
	uint32_t blazer_speed_limit_id = node_id << 8 | CAN_GET_SPEED_LIMIT;

	CAN_SendMessage(blazer_speed_limit_id);
}

/**
   * @brief  ��ȡ������ٶ�
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_acc(uint32_t node_id)
{
	uint32_t blazer_speed_acc_id = node_id << 8 | CAN_GET_ACC;

	CAN_SendMessage(blazer_speed_acc_id);
}

/**
   * @brief  ��ȡ������ٶ�
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_dec(uint32_t node_id)
{
	uint32_t blazer_speed_dec_id = node_id << 8 | CAN_GET_DEC;

	CAN_SendMessage(blazer_speed_dec_id);
}

/**
   * @brief  ��ȡ�ٶȻ�kp
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_kp(uint32_t node_id)
{
	uint32_t blazer_speed_kp_id = node_id << 8 | CAN_GET_SPEED_KP;

	CAN_SendMessage(blazer_speed_kp_id);
}

/**
   * @brief  ��ȡ�ٶȻ�ki
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_ki(uint32_t node_id)
{
	uint32_t blazer_speed_ki_id = node_id << 8 | CAN_GET_SPEED_KI;

	CAN_SendMessage(blazer_speed_ki_id);
}

/**
   * @brief  ��ȡλ�û�kp
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_pos_kp(uint32_t node_id)
{
	uint32_t blazer_pos_kp_id = node_id << 8 | CAN_GET_POS_KP;

	CAN_SendMessage(blazer_pos_kp_id);
}

/**
   * @brief  ��ȡλ�û�ki
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_pos_ki(uint32_t node_id)
{
	uint32_t blazer_pos_ki_id = node_id << 8 | CAN_GET_POS_KI;

	CAN_SendMessage(blazer_pos_ki_id);
}

/**
   * @brief  ��ȡCAN����ʱ��
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_can_hb(uint32_t node_id)
{
	uint32_t blazer_can_hb_id = node_id << 8 | CAN_GET_CAN_HB;

	CAN_SendMessage(blazer_can_hb_id);
}

/**
   * @brief  ��ȡĸ�ߵ�ѹ
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_vbus(uint32_t node_id)
{
	uint32_t blazer_vbus_id = node_id << 8 | CAN_GET_VBUS;

	CAN_SendMessage(blazer_vbus_id);
}

/**
   * @brief  ��ȡĸ�ߵ���
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_ibus(uint32_t node_id)
{
	uint32_t blazer_ibus_id = node_id << 8 | CAN_GET_IBUS;

	CAN_SendMessage(blazer_ibus_id);
}

/**
   * @brief  ��ȡA�����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_ia(uint32_t node_id)
{
	uint32_t blazer_ia_id = node_id << 8 | CAN_GET_IA;

	CAN_SendMessage(blazer_ia_id);
}

/**
   * @brief  ��ȡB�����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_ib(uint32_t node_id)
{
	uint32_t blazer_ib_id = node_id << 8 | CAN_GET_IB;

	CAN_SendMessage(blazer_ib_id);
}

/**
   * @brief  ��ȡC�����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_ic(uint32_t node_id)
{
	uint32_t blazer_ic_id = node_id << 8 | CAN_GET_IC;

	CAN_SendMessage(blazer_ic_id);
}

/**
   * @brief  ��ȡd�����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_id(uint32_t node_id)
{
	uint32_t blazer_id_id = node_id << 8 | CAN_GET_ID;

	CAN_SendMessage(blazer_id_id);
}

/**
   * @brief  ��ȡq�����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_iq(uint32_t node_id)
{
	uint32_t blazer_iq_id = node_id << 8 | CAN_GET_VBUS;
	
	CAN_SendMessage(blazer_iq_id);
}

/**
   * @brief  ��ȡ�ٶ��˲�ֵ
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_speed_filt(uint32_t node_id)
{
	uint32_t blazer_speed_filt_id = node_id << 8 | CAN_GET_SPEED_FILT;
	
	CAN_SendMessage(blazer_speed_filt_id);
}

/**
   * @brief  ��ȡ������ԭʼֵ
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_encoder_raw(uint32_t node_id)
{
	uint32_t blazer_encoder_raw_id = node_id << 8 | CAN_GET_ENCODER_RAW;
	
	CAN_SendMessage(blazer_encoder_raw_id);
}

/**
   * @brief  ��ȡ�����¶�
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_temp(uint32_t node_id)
{
	uint32_t blazer_temp_id = node_id << 8 | CAN_GET_TEMP;
	
	CAN_SendMessage(blazer_temp_id);
}

/**
   * @brief  ��ȡ��ǰ����
   * @param  node_id �����ڵ�ID 
   * @retval ��
   */
void get_blazer_error(uint32_t node_id)
{
	uint32_t blazer_error_id = node_id << 8 | CAN_GET_ERROR;
	
	CAN_SendMessage(blazer_error_id);
}
