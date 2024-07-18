#include "analog.h"

AnalogParam_TypeDef AnalogParam_t;
extern FOC_TypeDef FOC_Sensored_t;
extern FOC_State FOC_State_t;
/*NTC热敏电阻 温度与电阻值对应表*/
/*1-100摄氏度 电阻(kohm)*/
const float NTC_Res[100]=
{
	/*1-10*/
	31.179f,29.636f,28.178f,26.800f,25.497f,24.263f,23.096f,21.992f,20.947f,19.958f,
	/*11-20*/
	19.022f,18.135f,17.294f,16.498f,15.742f,15.025f,14.345f,13.699f,13.086f,12.504f,
	/*21-30*/
	11.951f,11.426f,10.926f,10.452f,10.000f,9.570f,9.162f,8.773f,8.402f,8.049f,
	/*31-40*/
	7.713f,7.393f,7.088f,6.797f,6.520f,6.255f,6.003f,5.762f,5.532f,5.313f,
	/*41-50*/
	5.103f,4.903f,4.711f,4.529f,4.354f,4.187f,4.027f,3.874f,3.728f,3.588f,
	/*51-60*/
	3.454f,3.326f,3.203f,3.086f,2.973f,2.865f,2.761f,2.662f,2.567f,2.476f,
	/*61-70*/
	2.388f,2.304f,2.224f,2.146f,2.072f,2.001f,1.932f,1.866f,1.803f,1.742f,
	/*71-80*/
	1.684f,1.628f,1.574f,1.522f,1.472f,1.424f,1.378f,1.333f,1.290f,1.249f,
	/*81-90*/
	1.209f,1.171f,1.134f,1.099f,1.065f,1.032f,1.000f,0.969f,0.940f,0.911f,
	/*91-100*/
	0.884f,0.857f,0.831f,0.807f,0.783f,0.760f,0.738f,0.716f,0.695f,0.675f
};

uint16_t temp_adc;
float ntc_res;
void Temperature_Update(void)
{
	HAL_ADC_Start(&hadc1);
	temp_adc=(uint16_t)HAL_ADC_GetValue(&hadc1);
	ntc_res=(float)((4095-temp_adc)/temp_adc)*3.3f;
	
	uint8_t i=0;
	
	while(ntc_res<NTC_Res[i])
	{
		i++;
	}
	AnalogParam_t.temperature=i-1;
}

void Vbus_Update(void)
{
	static uint32_t adc_sum;
	static uint8_t i=0;
	
	if(i<10)
	{
		adc_sum+=ADC1->JDR4;
		i++;
	}
	else
	{
		adc_sum/=10;
		AnalogParam_t.vbus=(float)adc_sum/4095.0f*3.3f*11.0f;
		adc_sum=0;
		i=0;
	}
	
	/*欠压过压保护*/
	if(AnalogParam_t.vbus<=10.0f||AnalogParam_t.vbus>=27.0f)
	{
		//FOC_State_t=FOC_Wait;
	}
}

void Ibus_Update(void)
{
	AnalogParam_t.ibus=2.0f/3.0f*(FOC_Sensored_t.Ud * FOC_Sensored_t.Id +
								  FOC_Sensored_t.Uq * FOC_Sensored_t.Iq )/
								  AnalogParam_t.vbus;
}
