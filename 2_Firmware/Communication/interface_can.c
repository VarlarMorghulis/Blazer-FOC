#include "interface_can.h"

ReceiveMsg_TypeDef ReceiveMsg_t=
{
	.NodeID=0x01,
	.ParamID=0x00
};

uint8_t CAN_Rxflag=0;

extern FOC_State FOC_State_t;
extern PID_TypeDef PID_Id;
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

void CAN_Param_Handle(uint8_t param_id,float data)
{
	switch(param_id)
	{
		case CAN_SET_MRPM:
			PID_Speed.ref_value = data / 60.0f * _2PI;
		break;
		
		default:break;
	}
}

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
	node_id  = CAN_RxHeaderStruct.ExtId & 0x700;
	/*��ȡ��8λ*/
	param_id = CAN_RxHeaderStruct.ExtId & 0x0FF;
	
	/*���յ��Ľڵ�ID�������Ľڵ�ID��Ӧ*/
	if(node_id == ReceiveMsg_t.NodeID)
	{
		/*������־λ����*/
		CAN_Rx_timeout=0;
		
		if(CAN_Rxflag==0)
		{	/*�ибջ�ģʽ*/
			FOC_State_t=FOC_Sensored;
			CAN_Rxflag=1;
		}
		
		int_data |= rx_data[0] << 24;
		int_data |= rx_data[1] << 16;
		int_data |= rx_data[2] << 8;
		int_data |= rx_data[3];
		
		ReceiveMsg_t.ParamID=param_id;
		ReceiveMsg_t.data=(float)int_data;
		
		CAN_Param_Handle(ReceiveMsg_t.ParamID,ReceiveMsg_t.data);
		
		LED_G_TOGGLE;
	}
}


	
void CAN_LostConnect_Handle(void)
{
	/*��ʱ����յ��֮ǰ����������,��ֹ�������Ӻ����*/
	Clear_PID_Param(&PID_Id);
	Clear_PID_Param(&PID_Iq);
	Clear_PID_Param(&PID_Speed);
	
	LED_G(0);
	/*��CAN���ձ�־λ��0*/
	CAN_Rxflag=0;
	/*��������״̬*/
	FOC_State_t=FOC_Wait;
	
}

void CAN_ID_Conflict_Detect(void)
{
	
}
