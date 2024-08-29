#ifndef __INTERFACE_CAN_H__
#define __INTERFACE_CAN_H__

#include "main.h"
#include "can.h"

#define Motor_Num	4

#define READ_ONLY	0
#define WRITE_ONLY 	1

typedef enum
{
	CAN_SET_START_ENABLE=0,
	CAN_SET_CURRENT,
	CAN_SET_MRPM,
	CAN_SET_POS,
	CAN_SET_NODE_ID,
	CAN_SET_POLEPARIS,
	CAN_SET_ENCODER_TYPE,
	CAN_SET_MAX_CURRENT,
	CAN_SET_MAX_MRPM,
	CAN_SET_MAX_ACC,
	CAN_CALIB,
}CAN_PARAM_ID;

typedef struct
{
	float float_data;
	uint8_t u8_data[4];
}float_to_u8;

typedef struct
{
	uint8_t  data_type;
	uint8_t  node_id;
	uint8_t* data_ptr;
}CAN_Data_TypeDef;

void CAN_Filter_Init(void);
void CAN_SendMessage(uint32_t ID);
void set_blazer_node_id(uint32_t node_id,float node_id_set);
void set_blazer_pole_pairs(uint32_t node_id,float pole_pairs);
void set_blazer_calib(uint32_t node_id,float mode);

#endif