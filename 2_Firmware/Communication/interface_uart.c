#include "interface_uart.h"

Frame_TypeDef Frame_t=
{
	.tail = 0x7F800000//结束帧为固定的4个字节
};

extern FOC_TypeDef FOC;
extern MotorControl_TypeDef MotorControl;
extern Encoder_TypeDef Encoder;

void Vofa_Upload(void)
{	
	Frame_t.fdata[0] = (float)ADC1->JDR1;
	Frame_t.fdata[1] = (float)ADC1->JDR2;
	Frame_t.fdata[2] = (float)ADC1->JDR3;
	Frame_t.fdata[3] = FOC.Ia;
	Frame_t.fdata[4] = FOC.Ib;
	Frame_t.fdata[5] = FOC.Ic;
//	Frame_t.fdata[6]=FOC.Ibus_filt;
//	Frame_t.fdata[7]=SPI_Encoder_t.velocity;
//	Frame_t.fdata[8]=PID_Position.ref_value;
//	Frame_t.fdata[9]=PID_Position.samp_value;
	
	/*由于float和uint32_t都为4个字节,因此无需再做结构体数据对齐*/
	CDC_Transmit_FS((uint8_t *)&Frame_t, sizeof(Frame_TypeDef));
}



