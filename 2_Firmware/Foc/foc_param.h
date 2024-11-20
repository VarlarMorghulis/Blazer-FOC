#ifndef __FOC_PARAM_H__
#define __FOC_PARAM_H__

#include "common_inc.h"

typedef struct
{
	float node_id;
	
	float currentoffset_a,currentoffset_b,currentoffset_c;
	
	float motor_pole_pairs;
	float motor_phase_resistance;
	float motor_phase_inductance;
	float motor_flux;
	
	float encoder_type;
	float encoder_dir;
	float encoder_offset;
	float offset_lut[128];
	
	float id_kp;
	float id_ki;
	float iq_kp;
	float iq_ki;
	
	float speed_kp;
	float speed_ki;
	
	float pos_kp;
	float pos_ki;
	
	float calib_current;
	float current_limit;
	float speed_limit;
	float speedAcc;
	float speedDec;
	
	float can_hb;
}InterfaceParam_TypeDef;

void Param_Return_Default(void);
void Param_Upload(void);
void Param_Download(void);

#endif