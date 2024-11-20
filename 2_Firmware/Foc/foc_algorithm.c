#include "foc_algorithm.h"

FOC_TypeDef FOC;

extern MotorControl_TypeDef MotorControl;

/**
   * @brief  Park�任
   * @param  FOC�ṹ��ָ��
   * @retval ��
   */
void Park_Transform(float Ialpha, float Ibeta, float theta, float *Id, float *Iq)
{
	float sin = arm_sin_f32(theta);
	float cos = arm_cos_f32(theta);
 	*Id =  Ialpha * cos + Ibeta * sin;
	*Iq = -Ialpha * sin + Ibeta * cos;
}

/**
   * @brief  Park��任
   * @param  FOC�ṹ��ָ��
   * @retval ��
   */
void Inverse_Park_Transform(float mod_d, float mod_q, float theta, float *mod_alpha, float *mod_beta)
{
	float sin = arm_sin_f32(theta);
	float cos = arm_cos_f32(theta);
	*mod_alpha = mod_d * cos - mod_q * sin;
	*mod_beta  = mod_d * sin + mod_q * cos;
}

/**
   * @brief  Clarke�任
   * @param  FOC�ṹ��ָ��
   * @retval ��
   */
void Clarke_Transform(float Ia, float Ib, float Ic, float *Ialpha, float *Ibeta)
{
    *Ialpha = Ia;
    *Ibeta  = (Ib - Ic) * ONE_BY_SQRT_3;
}

void FOC_SVM(float alpha, float beta, float *tA, float *tB, float *tC ,int32_t *sector)
{
    if (beta >= 0.0f) {
        if (alpha >= 0.0f) {
            //quadrant I
            if (ONE_BY_SQRT_3 * beta > alpha)
                *sector = 2; //*sector v2-v3
            else
                *sector = 1; //*sector v1-v2

        } else {
            //quadrant II
            if (-ONE_BY_SQRT_3 * beta > alpha)
                *sector = 3; //*sector v3-v4
            else
                *sector = 2; //*sector v2-v3
        }
    } else {
        if (alpha >= 0.0f) {
            //quadrant IV
            if (-ONE_BY_SQRT_3 * beta > alpha)
                *sector = 5; //*sector v5-v6
            else
                *sector = 6; //*sector v6-v1
        } else {
            //quadrant III
            if (ONE_BY_SQRT_3 * beta > alpha)
                *sector = 4; //*sector v4-v5
            else
                *sector = 5; //*sector v5-v6
        }
    }

    switch (*sector) {
    // *sector v1-v2
    case 1: {
        // Vector on-times
        float t1 = alpha - ONE_BY_SQRT_3 * beta;
        float t2 = TWO_BY_SQRT_3 * beta;

        // PWM timings
        *tA = (1.0f - t1 - t2) * 0.5f;
        *tB = *tA + t1;
        *tC = *tB + t2;
    } break;

    // *sector v2-v3
    case 2: {
        // Vector on-times
        float t2 =  alpha + ONE_BY_SQRT_3 * beta;
        float t3 = -alpha + ONE_BY_SQRT_3 * beta;

        // PWM timings
        *tB = (1.0f - t2 - t3) * 0.5f;
        *tA = *tB + t3;
        *tC = *tA + t2;
    } break;

    // *sector v3-v4
    case 3: {
        // Vector on-times
        float t3 = TWO_BY_SQRT_3 * beta;
        float t4 = -alpha - ONE_BY_SQRT_3 * beta;

        // PWM timings
        *tB = (1.0f - t3 - t4) * 0.5f;
        *tC = *tB + t3;
        *tA = *tC + t4;
    } break;

    // *sector v4-v5
    case 4: {
        // Vector on-times
        float t4 = -alpha + ONE_BY_SQRT_3 * beta;
        float t5 = -TWO_BY_SQRT_3 * beta;

        // PWM timings
        *tC = (1.0f - t4 - t5) * 0.5f;
        *tB = *tC + t5;
        *tA = *tB + t4;
    } break;

    // *sector v5-v6
    case 5: {
        // Vector on-times
        float t5 = -alpha - ONE_BY_SQRT_3 * beta;
        float t6 =  alpha - ONE_BY_SQRT_3 * beta;

        // PWM timings
        *tC = (1.0f - t5 - t6) * 0.5f;
        *tA = *tC + t5;
        *tB = *tA + t6;
    } break;

    // *sector v6-v1
    case 6: {
        // Vector on-times
        float t6 = -TWO_BY_SQRT_3 * beta;
        float t1 = alpha + ONE_BY_SQRT_3 * beta;

        // PWM timings
        *tA = (1.0f - t6 - t1) * 0.5f;
        *tC = *tA + t1;
        *tB = *tC + t6;
    } break;
    }
}

static inline void Set_A_Duty(uint16_t Tcmp)
{
	TIM1->CCR1 = Tcmp;
}

static inline void Set_B_Duty(uint16_t Tcmp)
{
	TIM1->CCR2 = Tcmp;
}
static inline void Set_C_Duty(uint16_t Tcmp)
{
	TIM1->CCR3 = Tcmp;
}

void FOC_Voltage(float Vd_set, float Vq_set, float phase)
{
	FOC.Ia = MotorControl.ia;
	FOC.Ib = MotorControl.ib;
	FOC.Ic = MotorControl.ic;
	
	/*Clarke�任*/
	Clarke_Transform(FOC.Ia, FOC.Ib, FOC.Ic, &FOC.Ialpha, &FOC.Ibeta);
	
	/*Park�任*/
	Park_Transform(FOC.Ialpha, FOC.Ibeta, phase, &FOC.Id, &FOC.Iq);
	
	MotorControl.id = FOC.Id;
	MotorControl.iq = FOC.Iq;
	
    UTILS_LP_FAST(FOC.Iq_filt, FOC.Iq, 0.01f);
    UTILS_LP_FAST(FOC.Id_filt, FOC.Id, 0.01f);
	
	/*����ĸ�ߵ�ѹ���й�һ��*/
	float V_to_mod = 1.5f / FOC.Vbus_filt;
	FOC.mod_d 	   = V_to_mod * Vd_set;
	FOC.mod_q 	   = V_to_mod * Vq_set;
	
	/*ʸ�����Ʊ��ʹ���,�����޷�*/
    float mod_scalefactor = 0.95f * SQRT_3_BY_2 / sqrtf(fast_sq(FOC.mod_d) + fast_sq(FOC.mod_q));
    if (mod_scalefactor < 1.0f) 
	{
		FOC.mod_d  *= mod_scalefactor;
		FOC.mod_q  *= mod_scalefactor;
    }
	
	/*Park��任*/
	Inverse_Park_Transform(FOC.mod_d, FOC.mod_q, phase, &FOC.mod_alpha, &FOC.mod_beta);

	FOC_SVM(FOC.mod_alpha, FOC.mod_beta, &FOC.dtc_a, &FOC.dtc_b, &FOC.dtc_c, &FOC.sector);
	
	Set_A_Duty((uint16_t)(FOC.dtc_a * (float)PWM_TIM_PERIOD));
	Set_B_Duty((uint16_t)(FOC.dtc_b * (float)PWM_TIM_PERIOD));
	Set_C_Duty((uint16_t)(FOC.dtc_c * (float)PWM_TIM_PERIOD));
}

void FOC_Current(float Id_set, float Iq_set, float phase, float phase_vel)
{
	FOC.Ia = MotorControl.ia;
	FOC.Ib = MotorControl.ib;
	FOC.Ic = MotorControl.ic;
	
	/*Clarke�任*/
	Clarke_Transform(FOC.Ia, FOC.Ib, FOC.Ic, &FOC.Ialpha, &FOC.Ibeta);
	
	/*Park�任*/
	Park_Transform(FOC.Ialpha, FOC.Ibeta, phase, &FOC.Id, &FOC.Iq);
	
	MotorControl.id = FOC.Id;
	MotorControl.iq = FOC.Iq;
	
	/*����ĸ�ߵ�ѹ���й�һ��*/
	float V_to_mod = 1.5f / FOC.Vbus_filt;
	
	/*PI����*/
	float Ierr_d = Id_set - FOC.Id;
	float Ierr_q = Iq_set - FOC.Iq;
	FOC.mod_d  = V_to_mod * (FOC.Vd_int + Ierr_d * MotorControl.id_Kp);
	FOC.mod_q  = V_to_mod * (FOC.Vq_int + Ierr_q * MotorControl.iq_Kp);
	
	/*ʸ�����Ʊ��ʹ���,�����޷�*/
    float mod_scalefactor = 0.95f * SQRT_3_BY_2 / sqrtf(fast_sq(FOC.mod_d) + fast_sq(FOC.mod_q));
	if(mod_scalefactor < 1.0f)
	{
		FOC.mod_d  *= mod_scalefactor;
		FOC.mod_q  *= mod_scalefactor;
		FOC.Vd_int *= 0.99f;
		FOC.Vq_int *= 0.99f;
	}
	else
	{
		FOC.Vd_int += Ierr_d * MotorControl.id_Ki * Current_Ts;
		FOC.Vq_int += Ierr_q * MotorControl.iq_Ki * Current_Ts;
	}
	
	/*Park��任*/
	float pwm_phase = phase + phase_vel * Current_Ts;//�ǶȲ���
	Inverse_Park_Transform(FOC.mod_d, FOC.mod_q, pwm_phase, &FOC.mod_alpha, &FOC.mod_beta);
	
    UTILS_LP_FAST(FOC.Iq_filt, FOC.Iq, 0.01f);
    UTILS_LP_FAST(FOC.Id_filt, FOC.Id, 0.01f);
    FOC.Ibus = (FOC.mod_d * FOC.Id + FOC.mod_q * FOC.Iq);
    UTILS_LP_FAST(FOC.Ibus_filt, FOC.Ibus, 0.01f);
    FOC.Power_filt = FOC.Vbus_filt * FOC.Ibus_filt;
	
	MotorControl.ibus = FOC.Ibus_filt;
	
	FOC_SVM(FOC.mod_alpha, FOC.mod_beta, &FOC.dtc_a, &FOC.dtc_b, &FOC.dtc_c, &FOC.sector);
	
	Set_A_Duty((uint16_t)(FOC.dtc_a * (float)PWM_TIM_PERIOD));
	Set_B_Duty((uint16_t)(FOC.dtc_b * (float)PWM_TIM_PERIOD));
	Set_C_Duty((uint16_t)(FOC.dtc_c * (float)PWM_TIM_PERIOD));
}

/**
   * @brief  ���ͣת���ú���
   * @param  ��
   * @retval ��
   */
void PWM_TurnOnHighSides(void)
{
	TIM1->CCR1 = (uint16_t)PWM_TIM_PERIOD;
	TIM1->CCR2 = (uint16_t)PWM_TIM_PERIOD;
	TIM1->CCR3 = (uint16_t)PWM_TIM_PERIOD;
} 

/**
   * @brief  ���ɲ�����ú���
   * @param  ��
   * @retval ��
   */
void PWM_TurnOnLowSides(void)
{
	TIM1->CCR1 = 0;
	TIM1->CCR2 = 0;
	TIM1->CCR3 = 0;
} 
