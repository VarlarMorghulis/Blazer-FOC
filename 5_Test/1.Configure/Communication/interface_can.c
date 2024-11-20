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
   * @brief  �޸������ڵ�ID
   * @param  node_id ����ԭ�ڵ�ID 
             node_id_set �����½ڵ�ID ��Ч��Χ0x00-0x07
   * @retval ��
   */
void set_blazer_node_id(uint32_t node_id,float node_id_set)
{
	uint32_t blazer_nodeid_id = node_id << 8 | CAN_SET_NODE_ID;
	uint32_t id_u32;
	
	id_u32=FloatToIntBit(node_id_set);
	
	blazer_content_transform[node_id].u8_data[0] = id_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = id_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = id_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = id_u32;
	
	CAN_SendMessage(blazer_nodeid_id);
}

/**
   * @brief  ���õ��������
   * @param  node_id �����ڵ�ID 
             pole_pairs ������ ��Χ2-30
   * @retval ��
   */
void set_blazer_pole_pairs(uint32_t node_id,float pole_pairs)
{
	uint32_t blazer_pp_id = node_id << 8 | CAN_SET_POLEPARIS;
	uint32_t pole_pairs_u32;
	
	pole_pairs_u32=FloatToIntBit(pole_pairs);
	
	blazer_content_transform[node_id].u8_data[0] = pole_pairs_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = pole_pairs_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = pole_pairs_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = pole_pairs_u32;
	
	CAN_SendMessage(blazer_pp_id);
}

/**
   * @brief  ���ñ������ͺ�
   * @param  node_id �����ڵ�ID 
             enc_type �������ͺ� 0:TLE5012B 1:MT6816 Ĭ���趨ΪTLE5012B
   * @retval ��
   */
void set_blazer_encoder_type(uint32_t node_id,float enc_type)
{
	uint32_t blazer_enc_type_id = node_id << 8 | CAN_SET_ENCODER_TYPE;
	uint32_t enc_type_u32;
	
	enc_type_u32=FloatToIntBit(enc_type);
	
	blazer_content_transform[node_id].u8_data[0] = enc_type_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = enc_type_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = enc_type_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = enc_type_u32;
	
	CAN_SendMessage(blazer_enc_type_id);
}

/**
   * @brief  �趨У׼����
   * @param  node_id 	   �����ڵ�ID 
             current_cal   У׼����(A)һ��ȡ����������50%
   * @retval ��
   */
void set_blazer_current_cal(uint32_t node_id, float current_cal)
{
	uint32_t blazer_current_cal_id = node_id << 8 | CAN_SET_CURRENT_CAL;
	uint32_t current_cal_u32;
	
	current_cal_u32 = FloatToIntBit(current_cal);
	
	blazer_content_transform[node_id].u8_data[0] = current_cal_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = current_cal_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = current_cal_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = current_cal_u32;
	
	CAN_SendMessage(blazer_current_cal_id);
}

/**
   * @brief  �趨����������
   * @param  node_id 	   �����ڵ�ID 
             current_limit �趨����(A)
   * @retval ��
   */
void set_blazer_current_limit(uint32_t node_id, float current_limit)
{
	uint32_t blazer_current_limit_id = node_id << 8 | CAN_SET_CURRENT_LIMIT;
	uint32_t current_limit_u32;
	
	current_limit_u32 = FloatToIntBit(current_limit);
	
	blazer_content_transform[node_id].u8_data[0] = current_limit_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = current_limit_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = current_limit_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = current_limit_u32;
	
	CAN_SendMessage(blazer_current_limit_id);
}

/**
   * @brief  �������ת������
   * @param  node_ id    �����ڵ�ID 
             speed_limit ���ת��(r/s) �����еת��
   * @retval ��
   */
void set_blazer_speed_limit(uint32_t node_id,float speed_limit)
{
	uint32_t blazer_speed_limit_id = node_id << 8 | CAN_SET_SPEED_LIMIT;
	uint32_t speed_limit_u32;
	
	speed_limit_u32=FloatToIntBit(speed_limit);
	
	blazer_content_transform[node_id].u8_data[0] = speed_limit_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = speed_limit_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = speed_limit_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = speed_limit_u32;
	
	CAN_SendMessage(blazer_speed_limit_id);
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
   * @param  node_id  �����ڵ�ID 
             speed_kp �ٶȻ�kp
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
   * @param  node_id  �����ڵ�ID 
             speed_ki �ٶȻ�ki
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

/**
   * @brief  �趨λ�û�kp
   * @param  node_id  �����ڵ�ID 
             pos_kp   λ�û�kp
   * @retval ��
   */
void set_blazer_pos_kp(uint32_t node_id, float pos_kp)
{
	uint32_t blazer_pos_kp_id = node_id << 8 | CAN_SET_POS_KP;
	uint32_t pos_kp_u32;
	
	pos_kp_u32 = FloatToIntBit(pos_kp);
	
	blazer_content_transform[node_id].u8_data[0] = pos_kp_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = pos_kp_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = pos_kp_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = pos_kp_u32;
	
	CAN_SendMessage(blazer_pos_kp_id);
}

/**
   * @brief  �趨λ�û�ki
   * @param  node_id  �����ڵ�ID 
             pos_ki   λ�û�ki
   * @retval ��
   */
void set_blazer_pos_ki(uint32_t node_id, float pos_ki)
{
	uint32_t blazer_pos_ki_id = node_id << 8 | CAN_SET_POS_KI;
	uint32_t pos_ki_u32;
	
	pos_ki_u32 = FloatToIntBit(pos_ki);
	
	blazer_content_transform[node_id].u8_data[0] = pos_ki_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = pos_ki_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = pos_ki_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = pos_ki_u32;
	
	CAN_SendMessage(blazer_pos_ki_id);
}

/**
   * @brief  �趨CAN����ʱ��
   * @param  node_id  �����ڵ�ID 
             can_hb   ����ʱ��(ms) �趨Ϊ0���������
   * @retval ��
   */
void set_blazer_can_hb(uint32_t node_id, float can_hb)
{
	uint32_t blazer_can_hb_id = node_id << 8 | CAN_SET_CAN_HB;
	uint32_t can_hb_u32;
	
	can_hb_u32 = FloatToIntBit(can_hb);
	
	blazer_content_transform[node_id].u8_data[0] = can_hb_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = can_hb_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = can_hb_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = can_hb_u32;
	
	CAN_SendMessage(blazer_can_hb_id);
}

