#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include "spi.h"
#include "foc_algorithm.h"
#include "foc_param.h"
#include "foc_utils.h"
#include "foc_run.h"

#define ENC_SPI	 hspi2
#define ABZ_TIM	 TIM3

#define READ_STATUS      0x8001 //8000
#define READ_ANGLE_VALUE 0x8021 //8020
#define READ_SPEED_VALUE 0x8031 //8030

#define TLE_CS_ENABLE  ENC_CS_GPIO_Port->BSRR=(uint32_t)ENC_CS_Pin<<16U/*Ƭѡ�ŵ�ƽ����*/
#define TLE_CS_DISABLE ENC_CS_GPIO_Port->BSRR=ENC_CS_Pin        		/*Ƭѡ�ŵ�ƽ����*/

#define USE_SPI_ENCODER
//#define USE_ABZ_ENCODER

typedef struct
{
	/*�������ֱ���*/
	uint16_t resolution;
	/*��Ƕ���ƫ*/
	uint16_t zero_enc_offset;
	/*����������*/
	int8_t sensor_dir;
	/*Ȧ��*/
	int32_t loop;
	/*SPIģʽΪ��ȡԭʼֵ,ABZģʽΪ����ֵ*/
	uint16_t raw_value;
	/*��ǰ�Ƕ�,�ϴνǶ�,��ǰ�Ƕ����ϴνǶ�֮�� ��λ����rad*/
	float angle_now,angle_last,d_angle;
	/*�ٶ� ��λ��rad/s*/
	float velocity;
}Encoder_TypeDef;

uint16_t ReadTLE5012B(uint16_t Reg);
void TLE5012B_Update(Encoder_TypeDef *TLE5012B_t);
float TLE5012B_GetRawAngle(Encoder_TypeDef *TLE5012B_t);
float TLE5012B_GetCalAngle(Encoder_TypeDef *TLE5012B_t);
float TLE5012B_GetVelocity(Encoder_TypeDef *TLE5012B_t);
void ABZ_Resolution_Init(void);
void ABZ_Update(Encoder_TypeDef *ABZ_t);
float ABZ_GetCalAngle(Encoder_TypeDef *ABZ_t);
void ENC_Z_EXTIIRQHandler(void);
ErrorState Encoder_Cal(FOC_TypeDef *FOC_t,Encoder_TypeDef *Encoder_t,uint8_t Pole_Pairs);

#endif