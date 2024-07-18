#include "interface_uart.h"

Frame_TypeDef Frame_t=
{
	.tail=0x7F800000//结束帧为固定的4个字节
};

extern FOC_TypeDef	FOC_Encoder_Calibration_t;
extern FOC_TypeDef FOC_Sensored_t;
extern Encoder_TypeDef TLE5012B_t;
extern Encoder_TypeDef ABZ_t;
extern HFI_TypeDef HFI_t;
extern FOC_TypeDef FOC_HFI_t;
extern FOC_TypeDef FOC_Anticogging_Calibration_t;
extern FOC_TypeDef FOC_Openloop_t;
extern float Motor_Rs;

void Vofa_Upload(void)
{
	Frame_t.fdata[0]=(float)TLE5012B_t.raw_value;
	Frame_t.fdata[1]=(float)TIM1->CCR1;
	Frame_t.fdata[2]=(float)TIM1->CCR2;
	Frame_t.fdata[3]=(float)TIM1->CCR3;
	Frame_t.fdata[4]=(float)ADC1->JDR1;
	Frame_t.fdata[5]=(float)ADC1->JDR2;
	Frame_t.fdata[6]=(float)ADC1->JDR3;
//	Frame_t.fdata[7]=FOC_HFI_t.Iq_l;
//	Frame_t.fdata[8]=FOC_HFI_t.Id_h;
//	Frame_t.fdata[9]=FOC_HFI_t.Id_l;
//	Frame_t.fdata[10]=0.0f;
	
	/*高频注入变量查看*/
//	Frame_t.fdata[0]=HFI_t.inj_volt;
//	Frame_t.fdata[1]=FOC_HFI_t.Ia;
//	Frame_t.fdata[2]=FOC_HFI_t.Ib;
//	Frame_t.fdata[3]=FOC_HFI_t.Ic;
//	Frame_t.fdata[4]=FOC_HFI_t.Id;
//	Frame_t.fdata[5]=FOC_HFI_t.Iq;
//	Frame_t.fdata[6]=FOC_HFI_t.Id_h;
//	Frame_t.fdata[7]=FOC_HFI_t.Id_l;
//	Frame_t.fdata[8]=FOC_HFI_t.Iq_h;
//	Frame_t.fdata[9]=FOC_HFI_t.Iq_l;
//	Frame_t.fdata[10]=FOC_HFI_t.theta_el;
//	Frame_t.fdata[11]=0.0f;
	
	/*由于float和uint32_t都为4个字节,因此无需再做结构体数据对齐*/
	HAL_UART_Transmit_DMA(&huart1,(uint8_t *)&Frame_t,sizeof(Frame_TypeDef));
}
