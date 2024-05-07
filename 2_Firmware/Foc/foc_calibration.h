#ifndef __FOC_CALIBRATION_H__
#define __FOC_CALIBRATION_H__

#include "main.h"

typedef struct
{
	/*ABC����������ľ�̬ƫ��*/
	uint16_t A_Offset;
	uint16_t B_Offset;
	uint16_t C_Offset;
}CurrentOffset_TypeDef;

#include "common_inc.h"

void FOC_Task_Encoder_Calibration(void);
void FOC_Task_ADC_Calibration(void);
void FOC_Task_Calibration(void);
#endif