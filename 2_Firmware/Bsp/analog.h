#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "common_inc.h"

typedef struct
{
	uint8_t temperature;
	float vbus;
	float ibus;
}AnalogParam_TypeDef;

void Current_Cal(void);
float Get_Phase_Ia(void);
float Get_Phase_Ib(void);
float Get_Phase_Ic(void);
void Temperature_Update(void);
void Vbus_Update(void);

#endif