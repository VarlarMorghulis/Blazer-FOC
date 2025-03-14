#ifndef __FOC_CALIBRATION_H__
#define __FOC_CALIBRATION_H__

#include "main.h"

#define OFFSET_LUT_NUM   128U

typedef enum 
{
	CS_NULL = 0,
	
	CS_ADC_OFFSET_START,
	CS_ADC_OFFSET_LOOP,
	CS_ADC_OFFSET_END,
	
	CS_MOTOR_R_START,
	CS_MOTOR_R_LOOP,
	CS_MOTOR_R_END,
	
	CS_MOTOR_L_START,
	CS_MOTOR_L_LOOP,
	CS_MOTOR_L_END,
	
	CS_MOTOR_FLUX_START,
	CS_MOTOR_FLUX_LOOP,
	CS_MOTOR_FLUX_END,
	
	CS_DIR_START,
	CS_DIR_LOOP,
	CS_DIR_END,
	
	CS_ENCODER_START,
	CS_ENCODER_CW_LOOP,
	CS_ENCODER_CCW_LOOP,
	CS_ENCODER_END,
	
}CalibStep_TyepeDef;

#include "common_inc.h"

void Task_Calib_Currentoffset(void);
void Task_Calib_R_L_Flux(void);
void Task_Calib_Encoder(void);
void FOC_Task_Param_Save(void);
void FOC_Task_Calibration(void);

#endif