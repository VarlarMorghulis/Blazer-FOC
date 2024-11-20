#include "foc_param.h"

InterfaceParam_TypeDef InterfaceParam;

extern MotorControl_TypeDef MotorControl;
extern Encoder_TypeDef Encoder;
extern CurrentOffset_TypeDef CurrentOffset;
extern CANMsg_TypeDef CANMsg;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_ZeroSpeed;
extern PID_TypeDef PID_Position;

void Param_Return_Default(void)
{
	CANMsg.node_id 						= 0x00;
	
	CurrentOffset.A_Offset 				= 2048;
	CurrentOffset.B_Offset				= 2048;
	CurrentOffset.C_Offset 				= 2048;
	
	MotorControl.motor_pole_pairs 		= 7;
//	MotorControl.motor_phase_resistance = 0.0f;
//	MotorControl.motor_phase_inductance = 0.0f;
//	MotorControl.motor_flux 			= 0.0f;
	
	Encoder.encoder_type		   		= (Encoder_Type)TLE5012B;
	MotorControl.encoder_dir       		= 1;
	MotorControl.encoder_offset    		= 0;
	
//	for(int i = 0;i < 128;i ++)
//	MotorControl.offset_lut[i] 			= 0;
	
//	MotorControl.id_Kp 					= InterfaceParam.id_kp;
//	MotorControl.id_Ki 					= InterfaceParam.id_ki;
//	MotorControl.iq_Kp 					= InterfaceParam.iq_kp;
//	MotorControl.iq_Ki 					= InterfaceParam.iq_ki;
	
	MotorControl.speed_Kp 				= 0.05f;
	MotorControl.speed_Ki 				= 0.5f;
	
	MotorControl.pos_Kp 				= 0.05f;
	MotorControl.pos_Ki 				= 0.0f;
	
	MotorControl.calib_current 			= 10.0f;
	MotorControl.current_limit 			= 30.0f;
	MotorControl.speed_limit   			= 200.0f;
	MotorControl.speedAcc      			= 50.0f;
	MotorControl.speedDec      			= 50.0f;
	
	CANMsg.can_hb_set 					= 500;
	
	MotorControl.ModeNow = Save_Param;
}

void Param_Upload(void)
{
	volatile int i = 0;
	
	InterfaceParam.node_id				  = (float)CANMsg.node_id;
	
	InterfaceParam.currentoffset_a 		  = (float)CurrentOffset.A_Offset;
	InterfaceParam.currentoffset_b 		  = (float)CurrentOffset.B_Offset;
	InterfaceParam.currentoffset_c 		  = (float)CurrentOffset.C_Offset;
	
	InterfaceParam.motor_pole_pairs 	  = (float)MotorControl.motor_pole_pairs;
	InterfaceParam.motor_phase_resistance = MotorControl.motor_phase_resistance;
	InterfaceParam.motor_phase_inductance = MotorControl.motor_phase_inductance;
	InterfaceParam.motor_flux 			  = MotorControl.motor_flux;
	
	InterfaceParam.encoder_type 		  = (float)Encoder.encoder_type;
	InterfaceParam.encoder_dir 			  = (float)MotorControl.encoder_dir;
	InterfaceParam.encoder_offset 		  = (float)MotorControl.encoder_offset;
	
	for(i = 0;i < 128;i ++)
	InterfaceParam.offset_lut[i] 		  = (float)MotorControl.offset_lut[i];
	
	InterfaceParam.id_kp 				  = MotorControl.id_Kp;
	InterfaceParam.id_ki 				  = MotorControl.id_Ki;
	InterfaceParam.iq_kp 				  = MotorControl.iq_Kp;
	InterfaceParam.iq_ki 				  = MotorControl.iq_Ki;
	
	InterfaceParam.speed_kp 			  = MotorControl.speed_Kp;
	InterfaceParam.speed_ki 			  = MotorControl.speed_Ki;
	
	InterfaceParam.pos_kp 				  = MotorControl.pos_Kp;
	InterfaceParam.pos_ki 				  = MotorControl.pos_Ki ;
	
	InterfaceParam.calib_current 		  = MotorControl.calib_current;
	InterfaceParam.current_limit 		  = MotorControl.current_limit;
	InterfaceParam.speed_limit 			  = MotorControl.speed_limit;
	InterfaceParam.speedAcc 			  = MotorControl.speedAcc;
	InterfaceParam.speedDec 			  = MotorControl.speedDec;
	
	InterfaceParam.can_hb 				  = (float)CANMsg.can_hb_set;
}

void Param_Download(void)
{
	volatile int i = 0;
	
	CANMsg.node_id 						= (uint8_t)InterfaceParam.node_id;
	
	CurrentOffset.A_Offset 				= (uint16_t)InterfaceParam.currentoffset_a;
	CurrentOffset.B_Offset 				= (uint16_t)InterfaceParam.currentoffset_b;
	CurrentOffset.C_Offset 				= (uint16_t)InterfaceParam.currentoffset_c;
	
	MotorControl.motor_pole_pairs 		= (int32_t)InterfaceParam.motor_pole_pairs;
	MotorControl.motor_phase_resistance = InterfaceParam.motor_phase_resistance;
	MotorControl.motor_phase_inductance = InterfaceParam.motor_phase_inductance;
	MotorControl.motor_flux 			= InterfaceParam.motor_flux;
	
	Encoder.encoder_type		   		= (Encoder_Type)InterfaceParam.encoder_type;
	MotorControl.encoder_dir       		= (int32_t)InterfaceParam.encoder_dir;
	MotorControl.encoder_offset    		= (int32_t)InterfaceParam.encoder_offset;
	
	for(i = 0;i < 128;i ++)
	MotorControl.offset_lut[i] 			= (int32_t)InterfaceParam.offset_lut[i];
	
	MotorControl.id_Kp 					= InterfaceParam.id_kp;
	MotorControl.id_Ki 					= InterfaceParam.id_ki;
	MotorControl.iq_Kp 					= InterfaceParam.iq_kp;
	MotorControl.iq_Ki 					= InterfaceParam.iq_ki;
	
	MotorControl.speed_Kp 				= InterfaceParam.speed_kp;
	MotorControl.speed_Ki 				= InterfaceParam.speed_ki;
	
	MotorControl.pos_Kp 				= InterfaceParam.pos_kp;
	MotorControl.pos_Ki 				= InterfaceParam.pos_ki;
	
	MotorControl.calib_current 			= InterfaceParam.calib_current;
	MotorControl.current_limit 			= InterfaceParam.current_limit;
	MotorControl.speed_limit   			= InterfaceParam.speed_limit;
	MotorControl.speedAcc      			= InterfaceParam.speedAcc;
	MotorControl.speedDec      			= InterfaceParam.speedDec;
	
	CANMsg.can_hb_set 					= (int32_t)InterfaceParam.can_hb;
}
