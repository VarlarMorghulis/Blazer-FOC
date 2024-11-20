#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include <stdbool.h>
#include "spi.h"
#include "foc_algorithm.h"
#include "foc_param.h"
#include "foc_utils.h"
#include "foc_errhandle.h"
#include "foc_run.h"

/*�������ͺ�*/
typedef enum 
{	
	TLE5012B,
	MT6816
}Encoder_Type;

typedef struct
{
	/*�������ͺ�*/
	Encoder_Type encoder_type;
	
	/*�������ֱ�λ��*/
	int32_t resolution;
	
	/*������CPR תһȦ�������Χ*/
	int32_t cpr;

	/*ԭʼֵ*/
    int raw;
    int count_in_cpr;
    int count_in_cpr_prev;

    int64_t shadow_count;

    // pll use
    float pos_cpr_counts;
    float vel_estimate_counts;

    float pos;
    float vel;

	/*��Ƕ� (rad)*/
    float phase;
	/*����ٶ� (rad/s)*/
    float phase_vel;

    float pll_kp;
    float pll_ki;
    float interpolation;
    float snap_threshold;
}Encoder_TypeDef;



#define enc_spi	 hspi2
#define ABZ_TIM	 TIM3

#define READ_STATUS      0x8001 //8000
#define READ_ANGLE_VALUE 0x8021 //8020
#define READ_SPEED_VALUE 0x8031 //8030

#define ENC_CS_ENABLE  ENC_CS_GPIO_Port->BSRR=(uint32_t)ENC_CS_Pin<<16U/*Ƭѡ�ŵ�ƽ����*/
#define ENC_CS_DISABLE ENC_CS_GPIO_Port->BSRR=ENC_CS_Pin        		/*Ƭѡ�ŵ�ƽ����*/

#define USE_SPI_ENCODER
//#define USE_ABZ_ENCODER


uint16_t ReadTLE5012B_Raw(uint16_t Reg);
uint16_t ReadMT6816_Raw(void);

void Encoder_ParamInit(void);
uint16_t ReadSPIEncoder_Raw(void);
void Encoder_Update(void);
float Encoder_GetEleVel(void);
float Encoder_GetMecVel(void);
float Encoder_GetElePhase(void);
float Encoder_GetMecPhase(void);

void ABZ_Resolution_Init(void);

#endif