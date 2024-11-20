#include "foc_calibration.h"

#define MAX_MOTOR_POLE_PAIRS 30U
#define SAMPLES_PER_PPAIR    128U

//static int      *p_error_arr = NULL;
int  p_error_arr[MAX_MOTOR_POLE_PAIRS * SAMPLES_PER_PPAIR];
CalibStep_TyepeDef CalibStep = CS_NULL;

extern FOC_TypeDef FOC;
extern MotorControl_TypeDef MotorControl;
extern CurrentOffset_TypeDef CurrentOffset;
extern Encoder_TypeDef Encoder;
extern InterfaceParam_TypeDef InterfaceParam;

void Task_Calib_R_L_Flux(void)
{
	static int loop_count;
	
    // R
    static const float     kI           = 2.0f;
    static const uint32_t num_R_cycles = FOC_FREQ * 2;

    // L
    static float           Ialphas[2];
    static float           voltages[2];
    static const uint32_t num_L_cycles = FOC_FREQ / 2;

	float time = (float) loop_count * Current_Ts;
	
	static float A_offset_sum,B_offset_sum,C_offset_sum;
	
    //const float voltage = MotorControl.calib_current * MotorControl.motor_phase_resistance * 3.0f / 2.0f;
	const float voltage = 3.0f;
	
	switch(CalibStep)
	{
		case CS_NULL:
			CalibStep = CS_ADC_OFFSET_START;
		break;
		
		case CS_ADC_OFFSET_START:
			loop_count = 0;
			time = 0;
			CalibStep = CS_ADC_OFFSET_LOOP;
		break;
		
		case CS_ADC_OFFSET_LOOP:
			A_offset_sum += (float)ADC1->JDR1;
			B_offset_sum += (float)ADC1->JDR2;
			C_offset_sum += (float)ADC1->JDR3;
			if(time >= 1.0f)
			{
				CalibStep = CS_ADC_OFFSET_END;
			}
		break;
		
		case CS_ADC_OFFSET_END:
			CurrentOffset.A_Offset = (uint16_t)(A_offset_sum / 20000.0f);
			CurrentOffset.B_Offset = (uint16_t)(B_offset_sum / 20000.0f);
			CurrentOffset.C_Offset = (uint16_t)(C_offset_sum / 20000.0f);
			
			A_offset_sum=0;
			B_offset_sum=0;
			C_offset_sum=0;
		
			if(CurrentOffset.A_Offset < 2028 || CurrentOffset.A_Offset > 2068 ||
			   CurrentOffset.B_Offset < 2028 || CurrentOffset.B_Offset > 2068 ||
			   CurrentOffset.C_Offset < 2028 || CurrentOffset.C_Offset > 2068	)
			{
				Set_ErrorNow(CurrentOffset_Error);
			}
			else
			{
				CalibStep = CS_MOTOR_R_START;
			}
			
		break;
		
		case CS_MOTOR_R_START:
			loop_count  = 0;
			voltages[0] = 0.0f;
			CalibStep  = CS_MOTOR_R_LOOP;
		break;
		
		case CS_MOTOR_R_LOOP:
			voltages[0] += kI * FOC_PERIOD * (MotorControl.calib_current - MotorControl.ia);

			// Test voltage along phase A
			FOC_Voltage(voltages[0], 0, 0);

			if (loop_count >= num_R_cycles) 
			{
				PWM_TurnOnLowSides();
				CalibStep = CS_MOTOR_R_END;
			}
		break;
		
		case CS_MOTOR_R_END:
			MotorControl.motor_phase_resistance = (voltages[0] / MotorControl.calib_current) * 2.0f / 3.0f;
			
			if(MotorControl.motor_phase_resistance > 0.15f)
			{
				Set_ErrorNow(Large_Phase_Resistance);
			}
			else
			{
				MotorControl.id_Ki = MotorControl.motor_phase_resistance * 200.0f;
				MotorControl.iq_Ki = MotorControl.motor_phase_resistance * 200.0f;
			
				CalibStep = CS_MOTOR_L_START;
			}
		break;
		
		case CS_MOTOR_L_START:
			loop_count  = 0;
			Ialphas[0]  = 0.0f;
			Ialphas[1]  = 0.0f;
			voltages[0] = -voltage;
			voltages[1] = +voltage;
			FOC_Voltage(voltages[0], 0, 0);
			CalibStep = CS_MOTOR_L_LOOP;
		break;
		
		case CS_MOTOR_L_LOOP:
		{	
			int i = loop_count & 1;
			Ialphas[i] += MotorControl.ia;

			// Test voltage along phase A
			FOC_Voltage(voltages[i], 0, 0);

			if (loop_count >= (num_L_cycles << 1)) 
			{
				PWM_TurnOnLowSides();
				CalibStep = CS_MOTOR_L_END;
			}
		}
		break;
		
		case CS_MOTOR_L_END:
		{
			float dI_by_dt                   = (Ialphas[1] - Ialphas[0]) / (float) (Current_Ts * num_L_cycles);
			float L                          = voltage / dI_by_dt;
			MotorControl.motor_phase_inductance = L * 2.0f / 3.0f;
			
			loop_count = 0;
			
			if(MotorControl.motor_phase_inductance > 0.0005f)
			{
				Set_ErrorNow(Large_Phase_Inductance);
			}
			else
			{
				MotorControl.id_Kp = MotorControl.motor_phase_inductance * 200.0f;
				MotorControl.iq_Kp = MotorControl.motor_phase_inductance * 200.0f;
				CalibStep  = CS_NULL;
				MotorControl.ModeNow = Save_Param;
			}
		}
		break;
		
		case CS_MOTOR_FLUX_START:
			
		break;
		
		case CS_MOTOR_FLUX_LOOP:
			
		break;
		
		case CS_MOTOR_FLUX_END:
			
		break;
		
		default:break;
	}
	loop_count ++;
}

void Task_Calib_Encoder(void)
{
	static int loop_count;
	
	static const float calib_phase_vel = _PI;
	
	static float phase_set;
	static float start_count;
	
    static int16_t sample_count;
    static float   next_sample_time;
	
	float time = (float) loop_count * Current_Ts;
	
    const float voltage = MotorControl.calib_current * MotorControl.motor_phase_resistance * 3.0f / 2.0f;
	//static float voltage_temp;
	
	switch(CalibStep)
	{
		case CS_NULL:
			CalibStep = CS_DIR_START;
		break;
		
		case CS_DIR_START:
			MotorControl.encoder_dir = +1;
			//voltage_temp = voltage * time / 2.0f;
			phase_set = 0.0f;
			FOC_Voltage(voltage, 0.0f, phase_set);
			if(time >=2.0f)
			{
				start_count = (float) Encoder.shadow_count;
				CalibStep = CS_DIR_LOOP;
				break;
			}
		break;
		
		case CS_DIR_LOOP:
			phase_set += calib_phase_vel * Current_Ts;
			FOC_Voltage(voltage, 0.0f, phase_set);
			if(phase_set >= 4.0f * _2PI)
				CalibStep = CS_DIR_END;
		
		break;
		
		case CS_DIR_END:
		{
			int32_t diff = Encoder.shadow_count - start_count;
			
			/*确定编码器方向*/
			if(diff > 0)
				MotorControl.encoder_dir = +1;
			else
				MotorControl.encoder_dir = -1;
			
			CalibStep = CS_ENCODER_START;
		}
		break;
		
		case CS_ENCODER_START:
			phase_set        = 0;
			loop_count       = 0;
			sample_count     = 0;
			next_sample_time = 0;
			CalibStep        = CS_ENCODER_CW_LOOP;	
		break;
		
		case CS_ENCODER_CW_LOOP:
			if (sample_count < (MotorControl.motor_pole_pairs * SAMPLES_PER_PPAIR)) 
			{
				if (time > next_sample_time) 
				{
					next_sample_time += _2PI / ((float) SAMPLES_PER_PPAIR * calib_phase_vel);

					int count_ref = (phase_set * (float)Encoder.cpr) / (_2PI * (float) MotorControl.motor_pole_pairs);
					int error     = Encoder.raw - count_ref;
					error += Encoder.cpr * (error < 0);
					p_error_arr[sample_count] = error;

					sample_count++;
				}

				phase_set += calib_phase_vel * Current_Ts;
			} 
			else 
			{
				phase_set -= calib_phase_vel * Current_Ts;
				loop_count = 0;
				sample_count--;
				next_sample_time = 0;
				CalibStep       = CS_ENCODER_CCW_LOOP;
				break;
			}
			FOC_Voltage(voltage, 0.0f, phase_set);
		break;
		
		case CS_ENCODER_CCW_LOOP:
			if (sample_count >= 0) 
			{
				if (time > next_sample_time) 
				{
					next_sample_time += _2PI / ((float) SAMPLES_PER_PPAIR * calib_phase_vel);

					int count_ref = (phase_set * (float)Encoder.cpr) / (_2PI * (float) MotorControl.motor_pole_pairs);
					int error     = Encoder.raw - count_ref;
					error += Encoder.cpr * (error < 0);
					p_error_arr[sample_count] = (p_error_arr[sample_count] + error) / 2;

					sample_count--;
				}

				phase_set -= calib_phase_vel * Current_Ts;
			} 
			else 
			{
				PWM_TurnOnLowSides();
				CalibStep = CS_ENCODER_END;
				break;
			}
			FOC_Voltage(voltage, 0.0f, phase_set);
		break;
		
		case CS_ENCODER_END:
		{
			// Calculate average offset
			int64_t moving_avg = 0;
			for (int i = 0; i < (MotorControl.motor_pole_pairs * SAMPLES_PER_PPAIR); i++) 
			{
				moving_avg += p_error_arr[i];
			}
			MotorControl.encoder_offset = moving_avg / (MotorControl.motor_pole_pairs * SAMPLES_PER_PPAIR);

			// FIR and map measurements to lut
			int window     = SAMPLES_PER_PPAIR;
			int lut_offset = p_error_arr[0] * OFFSET_LUT_NUM / Encoder.cpr;
			for (int i = 0; i < OFFSET_LUT_NUM; i++) 
			{
				moving_avg = 0;
				for (int j = (-window) / 2; j < (window) / 2; j++) 
				{
					int index = i * MotorControl.motor_pole_pairs * SAMPLES_PER_PPAIR / OFFSET_LUT_NUM + j;
					if (index < 0) 
					{
						index += (SAMPLES_PER_PPAIR * MotorControl.motor_pole_pairs);
					} 
					else if (index > (SAMPLES_PER_PPAIR * MotorControl.motor_pole_pairs - 1)) 
					{
						index -= (SAMPLES_PER_PPAIR * MotorControl.motor_pole_pairs);
					}
					moving_avg += p_error_arr[index];
				}
				moving_avg    = moving_avg / window;
				int lut_index = lut_offset + i;
				lut_index = lut_offset + i;
				if(lut_index > (OFFSET_LUT_NUM - 1)) 
				{
					lut_index -= OFFSET_LUT_NUM;
				}
				MotorControl.offset_lut[lut_index] = moving_avg - MotorControl.encoder_offset;
			}
			
			loop_count           = 0;
			sample_count         = 0;
			next_sample_time     = 0;
			CalibStep            = CS_NULL;
			MotorControl.ModeNow = Save_Param;
		}
		break;
		
		default:break;
	}
	
	loop_count++;
}
