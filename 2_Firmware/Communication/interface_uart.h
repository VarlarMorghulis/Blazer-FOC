#ifndef __INTERFACE_UART_H__
#define __INTERFACE_UART_H__

#include "common_inc.h"

//通道数量
#define CH_COUNT	7
typedef struct
{
	float fdata[CH_COUNT];
	uint32_t tail;
}Frame_TypeDef;

void Vofa_Upload(void);

#endif