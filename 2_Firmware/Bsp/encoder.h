#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "main.h"
#include "spi.h"
#include "foc_algorithm.h"
#include "foc_param.h"
#include "foc_utils.h"
#include "foc_run.h"

/*编码器型号*/
typedef enum 
{	
	TLE5012B,
	MT6816
}Encoder_Type;

typedef struct
{
	/*编码器型号*/
	Encoder_Type enc_type;
	/*编码器分辨位数*/
	uint8_t resolution;
	/*编码器CPR 转一圈的输出范围*/
	uint16_t cpr;
	/*电角度零偏*/
	uint16_t zero_enc_offset;
	/*编码器极性*/
	int8_t sensor_dir;
	/*圈数*/
	int32_t loop;
	/*SPI模式为读取原始值,ABZ模式为计数值*/
	uint16_t raw_value;
	/*经过偏心补偿后的值*/
	uint16_t compensate_value;
	/*当前角度,上次角度,当前角度与上次角度之差 单位都是rad*/
	float angle_now,angle_last,d_angle;
	/*速度 单位是rad/s*/
	float velocity;
	/*进入闭环的标志位*/
	uint8_t closeloop_flag;
	/*偏心补偿的标志位*/
	uint8_t compensate_flag;
}Encoder_TypeDef;



#define enc_spi	 hspi2
#define ABZ_TIM	 TIM3

#define READ_STATUS      0x8001 //8000
#define READ_ANGLE_VALUE 0x8021 //8020
#define READ_SPEED_VALUE 0x8031 //8030

#define ENC_CS_ENABLE  ENC_CS_GPIO_Port->BSRR=(uint32_t)ENC_CS_Pin<<16U/*片选脚电平拉低*/
#define ENC_CS_DISABLE ENC_CS_GPIO_Port->BSRR=ENC_CS_Pin        		/*片选脚电平拉高*/

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