#include "interface_can.h"

SendMsg_TypeDef SendMsg_t;

ReceiveMsg_TypeDef ReceiveMsg_t=
{
	.ID=0x01
};

extern FOC_State FOC_State_t;
extern PID_TypeDef PID_Iq;
extern Encoder_TypeDef TLE5012B_t;
extern Encoder_TypeDef ABZ_t;
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
	/*�����յ���-16384 -- +16384ת��Ϊ-40A -- +40A���趨����*/
	PID_Iq.ref_value=((int16_t)(ReceiveMsg_t.given_current))*0.00244141f;
	
#ifdef USE_SPI_ENCODER
	/*���ͱ������Ƕ�ԭʼֵ*/
	SendMsg_t.angle=TLE5012B_t.raw_value;
	/*���ͱ������ٶ���rad/sתΪrpm*/
	SendMsg_t.speed=(uint16_t)((int16_t)(TLE5012B_t.velocity*9.549296585f));
#endif

#ifdef USE_ABZ_ENCODER
	/*���ͱ������Ƕ�ԭʼֵ*/
	SendMsg_t.angle=ABZ_t.raw_value;
	/*���ͱ������ٶ���rad/sתΪrpm*/
	SendMsg_t.speed=(uint16_t)((int16_t)(ABZ_t.velocity*9.549296585f));
#endif
	/*��-40A -- +40A�ĵ�ǰ����ת��Ϊ-16384 -- +16384�Ĵ���������*/
	SendMsg_t.real_current=(uint16_t)((int16_t)(FOC_Sensored_t.Iq*409.6f));
}

uint8_t CAN_Rxflag=0;

/**
   * @brief  CAN1���ݽ��պ���
   * @param  ��
   * @retval ��
   */
void CANRxIRQHandler(void)
{
	CAN_RxHeaderTypeDef CAN_RxHeaderStruct;
	uint8_t rx_data[2];
	
	HAL_CAN_GetRxMessage(&hcan1,CAN_RX_FIFO0,&CAN_RxHeaderStruct,rx_data);
	
	/*���ط��͵���չID��������ID��Ӧ*/
	if(CAN_RxHeaderStruct.ExtId==ReceiveMsg_t.ID+0x150)
	{
		/*�����ճ�ʱ��־λ����,���ƿ��Ź���ʵ�ַ�ʽ*/
		CAN_Rx_timeout=0;
		
#ifdef USE_ABZ_ENCODER
	if(Z_confirm_flag==1)
	{
		/*�ибջ�ģʽ*/
		FOC_State_t=FOC_Sensored;
		
		ReceiveMsg_t.given_current=0;
		ReceiveMsg_t.given_current|=rx_data[0]<<8*1;
		ReceiveMsg_t.given_current|=rx_data[1];
		
		CAN_DataTransform();
	}
#endif

#ifdef USE_SPI_ENCODER
	if(CAN_Rxflag==0)
	{	/*�ибջ�ģʽ*/
		FOC_State_t=FOC_Sensored;
		CAN_Rxflag=1;
	}
	
	ReceiveMsg_t.given_current=0;
	ReceiveMsg_t.given_current|=rx_data[0]<<8*1;
	ReceiveMsg_t.given_current|=rx_data[1];
		
	CAN_DataTransform();
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
	
	CAN_TxHeaderStruct.StdId=0;
	CAN_TxHeaderStruct.ExtId=ReceiveMsg_t.ID;
	CAN_TxHeaderStruct.IDE=CAN_ID_EXT;
	CAN_TxHeaderStruct.RTR=CAN_RTR_DATA;/*����֡*/
	CAN_TxHeaderStruct.DLC=0x08;/*8���ֽڵ��غ�����*/
	CAN_TxHeaderStruct.TransmitGlobalTime=ENABLE;
	
	CAN_DataTransform();
	
	/*ת�ӻ�е�Ƕȸ�8λ*/
	tx_data[0]=SendMsg_t.angle>>8;
	/*ת�ӻ�е�Ƕȵ�8λ*/
	tx_data[1]=SendMsg_t.angle;
	/*ת���ٶȸ�8λ*/
	tx_data[2]=SendMsg_t.speed>>8;
	/*ת���ٶȵ�8λ*/
	tx_data[3]=SendMsg_t.speed;
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
//	Motor_Release();
	LED_G(0);
	/*��CAN���ձ�־λ��0*/
	CAN_Rxflag=0;
	/*��������״̬*/
	FOC_State_t=FOC_Wait;
	
}

void CAN_ID_Conflict_Detect(void)
{
	
}
