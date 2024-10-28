#ifndef __FOC_CALIBRATION_H__
#define __FOC_CALIBRATION_H__

#include "main.h"

typedef struct
{
	/*ABC相电流采样的静态偏置*/
	uint16_t A_Offset;
	uint16_t B_Offset;
	uint16_t C_Offset;
}CurrentOffset_TypeDef;

typedef enum 
{	
	Calib_ADC=0,
	Calib_Encoder=1,
	Linearize_Encoder=2,
	Calib_Anticogging=3,
    Calib_Done=4
}Calib_State;

#include "common_inc.h"

void FOC_Task_ADC_Calibration(void);
void FOC_Task_Encoder_Calibration(void);
void FOC_Task_Param_Save(void);
void FOC_Task_Calibration(void);

#endif