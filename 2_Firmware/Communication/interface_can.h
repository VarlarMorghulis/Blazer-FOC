#ifndef __INTERFACE_CAN_H__
#define __INTERFACE_CAN_H__

#include "common_inc.h"

typedef enum
{
	CAN_SET_POLEPARIS=0,
	CAN_SET_MRPM,
	CAN_SET_NODE_ID,
	CAN_CALIB_ENCODER,
	CAN_SET_ENCODER_TYPE,
	CAN_SET_MAX_CURRENT,
	CAN_SET_MAX_MRPM,
	CAN_SET_MAX_ACC,
}CAN_PARAM_ID;
typedef struct
{
	/*节点ID*/
	uint8_t NodeID;
	/*参数ID*/
	uint8_t ParamID;
	float data;
}ReceiveMsg_TypeDef;

void CAN_Filter_Init(void);
void CAN_DataTransform(void);
void CANRxIRQHandler(void);
void CAN_SendMessage(void);
void CAN_LostConnect_Handle(void);

#endif