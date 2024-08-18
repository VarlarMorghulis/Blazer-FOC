#ifndef __INTERFACE_CAN_H__
#define __INTERFACE_CAN_H__

#include "common_inc.h"

typedef struct
{
	/*节点ID*/
	uint8_t NodeID;
	/*参数ID*/
	uint8_t ParamID;
	float data;
}ReceiveMsg_TypeDef;

typedef struct
{
	uint16_t enc;
	uint16_t speed_rpm;
	uint16_t real_current;
	uint8_t temperature;
}SendMsg_TypeDef;

void CAN_Filter_Init(void);
void CAN_DataTransform(void);
void CANRxIRQHandler(void);
void CAN_SendMessage(void);
void CAN_LostConnect_Handle(void);

#endif