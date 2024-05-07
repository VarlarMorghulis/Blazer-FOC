#ifndef __INTERFACE_CAN_H__
#define __INTERFACE_CAN_H__

#include "common_inc.h"

typedef struct
{
	uint8_t ID;
	uint16_t given_current;
}ReceiveMsg_TypeDef;

typedef struct
{
	uint16_t angle;
	uint16_t speed;
	uint16_t real_current;
	uint8_t temperature;
}SendMsg_TypeDef;

void CAN_Filter_Init(void);
void CAN_DataTransform(void);
void CANRxIRQHandler(void);
void CAN_SendMessage(void);

#endif