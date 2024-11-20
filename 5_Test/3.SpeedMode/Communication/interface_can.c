#include "interface_can.h"

float_to_u8 blazer_content_transform[Motor_Num];

CAN_Data_TypeDef CAN_Data[Motor_Num]=
{
	{0x00, (uint8_t*)(&blazer_content_transform[0].u8_data)},
	{0x01, (uint8_t*)(&blazer_content_transform[1].u8_data)},
	{0x02, (uint8_t*)(&blazer_content_transform[2].u8_data)},
	{0x03, (uint8_t*)(&blazer_content_transform[3].u8_data)},
	{0x04, (uint8_t*)(&blazer_content_transform[4].u8_data)},
	{0x05, (uint8_t*)(&blazer_content_transform[5].u8_data)},
	{0x06, (uint8_t*)(&blazer_content_transform[6].u8_data)},
	{0x07, (uint8_t*)(&blazer_content_transform[7].u8_data)},
};

uint32_t FloatToIntBit(float x)
{
	uint32_t *pInt;
	pInt = (uint32_t*)(&x);
	return *pInt;
}

float IntBitToFloat(uint32_t x)
{
	float * pFloat;
	pFloat = (float *)(&x);
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
		if(++ send_num == 10)
			break;
	}
}

/**
   * @brief  �趨����ģʽ
   * @param  node_id   �����ڵ�ID 
			 mode      ģʽ 
			 0:����״̬ 	1:����ģʽ 2:�ٶ�ģʽ 3:λ��ģʽ 4:У׼�����д��� 
		     5:У׼������ƫ�� 6:�ָ�Ĭ������ 7:���浱ǰ���� 8:�������
   * @retval ��
   */
void set_blazer_mode(uint32_t node_id, float mode)
{
	uint32_t blazer_mode_id = node_id << 8 | CAN_SET_MODE;
	uint32_t mode_u32;
	
	mode_u32 = FloatToIntBit(mode);
	
	blazer_content_transform[node_id].u8_data[0] = mode_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = mode_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = mode_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = mode_u32;
	
	CAN_SendMessage(blazer_mode_id);
}

/**
   * @brief  �趨���ת��
			 ת��Ϊ��еת��
   * @param  node_id �����ڵ�ID 
             speed   �趨�ٶ�(r/s)
   * @retval ��
   */
void set_blazer_speed(uint32_t node_id, float speed)
{
	uint32_t blazer_speed_id = node_id << 8 | CAN_SET_SPEED;
	uint32_t speed_u32;
	
	speed_u32 = FloatToIntBit(speed);
	
	blazer_content_transform[node_id].u8_data[0] = speed_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_u32;
	
	CAN_SendMessage(blazer_speed_id);
}

/**
   * @brief  �趨������ٶ�
   * @param  node_id   �����ڵ�ID 
             speed_acc �趨���ٶ�(r/(s*s))
   * @retval ��
   */
void set_blazer_speed_acc(uint32_t node_id, float speed_acc)
{
	uint32_t blazer_speed_acc_id = node_id << 8 | CAN_SET_ACC;
	uint32_t speed_acc_u32;
	
	speed_acc_u32 = FloatToIntBit(speed_acc);
	
	blazer_content_transform[node_id].u8_data[0] = speed_acc_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_acc_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_acc_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_acc_u32;
	
	CAN_SendMessage(blazer_speed_acc_id);
}

/**
   * @brief  �趨������ٶ�
   * @param  node_id   �����ڵ�ID 
             speed_dec �趨���ٶ�(r/(s*s))
   * @retval ��
   */
void set_blazer_speed_dec(uint32_t node_id, float speed_dec)
{
	uint32_t blazer_speed_dec_id = node_id << 8 | CAN_SET_DEC;
	uint32_t speed_dec_u32;
	
	speed_dec_u32 = FloatToIntBit(speed_dec);
	
	blazer_content_transform[node_id].u8_data[0] = speed_dec_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_dec_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_dec_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_dec_u32;
	
	CAN_SendMessage(blazer_speed_dec_id);
}


/**
   * @brief  �趨�ٶȻ�kp
   * @param  node_id �����ڵ�ID 
             kp 	 �ٶȻ�kp
   * @retval ��
   */
void set_blazer_speed_kp(uint32_t node_id, float speed_kp)
{
	uint32_t blazer_speed_kp_id = node_id << 8 | CAN_SET_SPEED_KP;
	uint32_t speed_kp_u32;
	
	speed_kp_u32 = FloatToIntBit(speed_kp);
	
	blazer_content_transform[node_id].u8_data[0] = speed_kp_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_kp_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_kp_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_kp_u32;
	
	CAN_SendMessage(blazer_speed_kp_id);
}

/**
   * @brief  �趨�ٶȻ�ki
   * @param  node_id �����ڵ�ID 
             kp 	 �ٶȻ�ki
   * @retval ��
   */
void set_blazer_speed_ki(uint32_t node_id, float speed_ki)
{
	uint32_t blazer_speed_ki_id = node_id << 8 | CAN_SET_SPEED_KI;
	uint32_t speed_ki_u32;
	
	speed_ki_u32 = FloatToIntBit(speed_ki);
	
	blazer_content_transform[node_id].u8_data[0] = speed_ki_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_ki_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_ki_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_ki_u32;
	
	CAN_SendMessage(blazer_speed_ki_id);
}
