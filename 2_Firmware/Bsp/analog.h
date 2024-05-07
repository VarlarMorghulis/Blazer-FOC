#ifndef __ANALOG_H__
#define __ANALOG_H__

#include "main.h"
#include "adc.h"
#include "foc_run.h"

typedef struct
{
	uint8_t temperature;
	float vbus;
	float ibus;
}AnalogParam_TypeDef;

void Temperature_Update(void);
void Vbus_Update(void);
void Ibus_Update(void);

#endif