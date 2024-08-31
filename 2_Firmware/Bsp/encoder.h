#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include "spi.h"
#include "foc_algorithm.h"
#include "foc_param.h"
#include "foc_utils.h"
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
	Encoder_Type enc_type;
	/*�������ֱ�λ��*/
	uint8_t resolution;
	/*������CPR תһȦ�������Χ*/
	uint16_t cpr;
	/*��Ƕ���ƫ*/
	uint16_t zero_enc_offset;
	/*����������*/
	int8_t sensor_dir;
	/*Ȧ��*/
	int32_t loop;
	/*SPIģʽΪ��ȡԭʼֵ,ABZģʽΪ����ֵ*/
	uint16_t raw_value;
	/*����ƫ�Ĳ������ֵ*/
	uint16_t compensate_value;
	/*��ǰ�Ƕ�,�ϴνǶ�,��ǰ�Ƕ����ϴνǶ�֮�� ��λ����rad*/
	float angle_now,angle_last,d_angle;
	/*�ٶ� ��λ��rad/s*/
	float velocity;
	/*����ջ��ı�־λ*/
	uint8_t closeloop_flag;
	/*ƫ�Ĳ����ı�־λ*/
	uint8_t compensate_flag;
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
void SPI_Encoder_Update(Encoder_TypeDef *SPI_Encoder_t);
float SPI_Encoder_GetRawAngle(Encoder_TypeDef *SPI_Encoder_t);
float SPI_Encoder_GetCalAngle(Encoder_TypeDef *SPI_Encoder_t);
float SPI_Encoder_GetVelocity(Encoder_TypeDef *SPI_Encoder_t);

void ABZ_Resolution_Init(void);
void ABZ_Update(Encoder_TypeDef *ABZ_Enc_t);
float ABZ_GetCalAngle(Encoder_TypeDef *ABZ_Enc_t);
void ENC_Z_EXTIIRQHandler(void);

ErrorState Encoder_Cal(FOC_TypeDef *FOC_t,Encoder_TypeDef *Encoder_t,uint8_t Pole_Pairs);

#endif