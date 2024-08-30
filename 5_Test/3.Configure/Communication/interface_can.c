#include "interface_can.h"

float_to_u8 blazer_content_transform[Motor_Num];

CAN_Data_TypeDef CAN_Data_t[Motor_Num]=
{
	{WRITE_ONLY , 0x00 , (uint8_t*)(&blazer_content_transform[0].u8_data)},
	{WRITE_ONLY , 0x01 , (uint8_t*)(&blazer_content_transform[1].u8_data)},
	{WRITE_ONLY , 0x02 , (uint8_t*)(&blazer_content_transform[2].u8_data)},
	{WRITE_ONLY , 0x03 , (uint8_t*)(&blazer_content_transform[3].u8_data)},
};

enum 
{	
	TLE5012B,
	MT6816
}Encoder_Type;

/*MT6816�ķ�ΧΪ0-16383*/
/*TLE5012B�ķ�ΧΪ0-32767*/

/**
   * @brief  CAN1��������ʼ������
   * @param  ��
   * @retval ��
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
   * @brief  CAN���ͱ���
   * @param  ID �ɽڵ�ID�Ͳ���ID���ɵĸ���ID
   * @retval ��
   */
void CAN_SendMessage(uint32_t ID)
{
	CAN_TxHeaderTypeDef CAN_TxHeaderStruct;
	uint8_t send_num=0;
	uint32_t pTxMailbox=0;
	
	CAN_TxHeaderStruct.StdId=0;
	CAN_TxHeaderStruct.ExtId=ID;
	CAN_TxHeaderStruct.IDE=CAN_ID_EXT;
	CAN_TxHeaderStruct.RTR=CAN_RTR_DATA;
	CAN_TxHeaderStruct.DLC=0x04;
	CAN_TxHeaderStruct.TransmitGlobalTime=ENABLE;
	
	while(HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeaderStruct,CAN_Data_t[ID>>8].data_ptr,&pTxMailbox)!=HAL_OK)
	{
		/*��������*/
		if(++send_num==10)
			break;
	}
}

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
   * @brief  �޸������ڵ�ID
   * @param  node_id ����ԭ�ڵ�ID 
             node_id_set �����½ڵ�ID
   * @retval ��
   */
void set_blazer_node_id(uint32_t node_id,float node_id_set)
{
	uint32_t blazer_node_id_id = node_id << 8 | CAN_SET_NODE_ID;
	uint32_t id_u32;
	
	id_u32=FloatToIntBit(node_id_set);
	
	blazer_content_transform[node_id].u8_data[0] = id_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = id_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = id_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = id_u32;
	
	CAN_SendMessage(blazer_node_id_id);
}

/**
   * @brief  ���õ��������
   * @param  node_id �����ڵ�ID 
             node_id_set �����½ڵ�ID
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
   * @brief  ��������������
   * @param  node_ id �����ڵ�ID 
             max_current ������(A) ʵ��ΪQ���������趨ֵ Ĭ���趨Ϊ40A
   * @retval ��
   */
void set_blazer_max_current(uint32_t node_id,float max_current)
{
	uint32_t blazer_max_current_id = node_id << 8 | CAN_SET_MAX_CURRENT;
	uint32_t max_current_u32;
	
	max_current_u32=FloatToIntBit(max_current);
	
	blazer_content_transform[node_id].u8_data[0] = max_current_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = max_current_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = max_current_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = max_current_u32;
	
	CAN_SendMessage(blazer_max_current_id);
}

/**
   * @brief  �����������ת��
   * @param  node_ id �����ڵ�ID 
             max_speed ���ת��(r/min) �����еת�� Ĭ���趨Ϊ8000r/min
   * @retval ��
   */
void set_blazer_max_speed(uint32_t node_id,float max_speed)
{
	uint32_t blazer_max_speed_id = node_id << 8 | CAN_SET_MAX_MRPM;
	uint32_t max_speed_u32;
	
	max_speed_u32=FloatToIntBit(max_speed);
	
	blazer_content_transform[node_id].u8_data[0] = max_speed_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = max_speed_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = max_speed_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = max_speed_u32;
	
	CAN_SendMessage(blazer_max_speed_id);
}

/**
   * @brief  ����У׼ģʽ
   * @param  node_id �����ڵ�ID 
             mode У׼ģʽ 0:У׼ADC�ͱ�������ƫ
   * @retval ��
   */
void set_blazer_calib(uint32_t node_id,float mode)
{
	uint32_t blazer_calib_id = node_id << 8 | CAN_CALIB;
	uint32_t mode_u32;
	
	mode_u32=FloatToIntBit(mode);
	
	blazer_content_transform[node_id].u8_data[0] = mode_u32 >> 24;
	blazer_content_transform[node_id].u8_data[1] = mode_u32 >> 16;
	blazer_content_transform[node_id].u8_data[2] = mode_u32 >> 8;
	blazer_content_transform[node_id].u8_data[3] = mode_u32;
	
	CAN_SendMessage(blazer_calib_id);
}