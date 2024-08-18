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
   * @brief  CAN1�˲�����ʼ������
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

void CAN_DataTransform(void)
{
	/*�����յ���-16384 -- +16384ת��Ϊ-40pi -- +40pi rad/s���趨ת��*/
	//PID_Speed.ref_value=((int16_t)(ReceiveMsg_t.given_speed))*20.0f*_2PI/16384.0f;
	/*�ٶ��޷�����*/
	PID_Speed.ref_value=_constrain(PID_Speed.ref_value,-125.663706f,125.663706f);
	
	/*��-40A -- +40A�ĵ�ǰ����ת��Ϊ-16384 -- +16384�Ĵ���������*/
	SendMsg_t.real_current=(uint16_t)((int16_t)(FOC_Sensored_t.Iq*40.0f/16384.0f));
	
#ifdef USE_SPI_ENCODER
	/*���ͱ������Ƕ�ԭʼֵ*/
	SendMsg_t.enc=SPI_Encoder_t.raw_value;
	/*���ͱ������ٶ���rad/sתΪrpm*/
	SendMsg_t.speed_rpm=(uint16_t)((int16_t)(SPI_Encoder_t.velocity*9.549296585f));
#endif

#ifdef USE_ABZ_ENCODER
	/*���ͱ������Ƕ�ԭʼֵ*/
	SendMsg_t.enc=ABZ_Enc_t.raw_value;
	/*���ͱ������ٶ���rad/sתΪrpm*/
	SendMsg_t.speed_rpm=(uint16_t)((int16_t)(ABZ_Enc_t.velocity*9.549296585f));
#endif

}

//void DataTransform(uint8_t param,)

/**
   * @brief  CAN1���ݽ��պ���
   * @param  ��
   * @retval ��
   */
void CANRxIRQHandler(void)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t node_id;
	uint8_t param_id;
	uint8_t rx_data[4];
	int32_t int_data=0;
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rx_data);
	
	/*��ȡ��3λ*/
	node_id  = CAN_RxHeaderStruct.StdId >> 8;
	/*��ȡ��8λ*/
	param_id = CAN_RxHeaderStruct.StdId & 0x0FF;
	
	/*���յ��Ľڵ�ID�������Ľڵ�ID��Ӧ*/
	if(node_id == ReceiveMsg_t.NodeID)
	{
		/*�����ճ�ʱ��־λ����,���ƿ��Ź���ʵ�ַ�ʽ*/
		CAN_Rx_timeout=0;

#ifdef USE_SPI_ENCODER
		if(CAN_Rxflag==0)
		{	/*�ибջ�ģʽ*/
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
		/*�ибջ�ģʽ*/
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
   * @brief  CAN1���ݷ����жϺ���
   * @param  ��
   * @retval ��
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
	CAN_TxHeaderStruct.RTR=CAN_RTR_DATA;/*����֡*/
	CAN_TxHeaderStruct.DLC=0x08;/*8���ֽڵ��غ�����*/
	CAN_TxHeaderStruct.TransmitGlobalTime=ENABLE;
	
	CAN_DataTransform();
	
	/*ת�ӻ�е�Ƕȸ�8λ*/
	tx_data[0]=SendMsg_t.enc>>8;
	/*ת�ӻ�е�Ƕȵ�8λ*/
	tx_data[1]=SendMsg_t.enc;
	/*ת���ٶȸ�8λ*/
	tx_data[2]=SendMsg_t.speed_rpm>>8;
	/*ת���ٶȵ�8λ*/
	tx_data[3]=SendMsg_t.speed_rpm;
	/*ʵ��ת�ص�����8λ*/
	tx_data[4]=SendMsg_t.real_current>>8;
	/*ʵ��ת�ص�����8λ*/
	tx_data[5]=SendMsg_t.real_current;
	/*�������¶�*/
	tx_data[6]=SendMsg_t.temperature;
	/*NULL*/
	tx_data[7]=0x00;
	
	while(HAL_CAN_AddTxMessage(&hcan1,&CAN_TxHeaderStruct,tx_data,&pTxMailbox)!=HAL_OK)
	{
		/*��������*/
		if(++send_num==10)
			break;
	}
}

void CAN_LostConnect_Handle(void)
{
	/*��ʱ����յ��֮ǰ����������,��ֹ�������Ӻ����*/
	PID_Iq.ref_value=0.0f;
	PID_Speed.ref_value=0.0f;
	
	LED_G(0);
	/*��CAN���ձ�־λ��0*/
	CAN_Rxflag=0;
	/*��������״̬*/
	FOC_State_t=FOC_Wait;
	
}

void CAN_ID_Conflict_Detect(void)
{
	
}
