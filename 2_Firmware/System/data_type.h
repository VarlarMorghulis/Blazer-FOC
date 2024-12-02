#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include "main.h"
#include <stdbool.h>

typedef enum
{
	Motor_Disable = 0,
	Current_Mode,
	Speed_Mode,
	Position_Mode,
	Calib_Motor_R_L_Flux,
	Calib_EncoderOffset,
	Default_Param,
	Save_Param,
	Clear_Error,
}ModeNow_TypeDef; 

typedef enum
{
	No_Error = 0,
	CurrentOffset_Error,
	Encoder_Error,
	PolePairs_Error,
	CAN_DisConnect,
	Large_Phase_Resistance,
	Large_Phase_Inductance,
	Over_Current,
	Over_Voltage,
	Under_Voltage,
	High_Temprature,
}ErrorNow_TypeDef;

typedef struct
{
	/*ABC相电流采样的静态偏置*/
	uint16_t A_Offset;
	uint16_t B_Offset;
	uint16_t C_Offset;
}CurrentOffset_TypeDef;

typedef struct
{
	ModeNow_TypeDef  ModeNow;
	ErrorNow_TypeDef ErrorNow;
	
	int32_t motor_pole_pairs;
	float motor_phase_resistance;
	float motor_phase_inductance;
	float motor_flux;
	
	int32_t encoder_dir;
	int32_t encoder_offset;
	int32_t offset_lut[128];
	
	bool isUseSensorless;
	
	float calib_current;
	float current_limit;
	float speed_limit;
	
	float speedAcc;
	float speedDec;
	
	float idRef;
	float id_Kp;
	float id_Ki;
	float iqRef;
	float iq_Kp;
	float iq_Ki;
	
	bool isUseSpeedRamp;
	bool isUseZeroSpeedTorqueMaintain;
	float speedRef;
	float speedShadow;
	float speed_Kp;
	float speed_Ki;
	
	float posRef;
	float pos_Kp;
	float pos_Ki;
	
	float vbus;
	float ibus;
	float temp;
	float ia,ib,ic;
	float id,iq;
	float ua,ub,uc;
}MotorControl_TypeDef;

#endif