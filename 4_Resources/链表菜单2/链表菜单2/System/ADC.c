#include "stm32f10x.h"                  // Device header

void ACD_Init(void)
{
	//配置时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//开启ADC的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//开启引脚GPIOA时钟
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC分频器六分频  72/6=12hz
	//配置GPIOA
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//配置结构体
	ADC_InitTypeDef ADC_Initstructure;
	ADC_Initstructure.ADC_Mode=ADC_Mode_Independent;//模式配置 独立模式
	ADC_Initstructure.ADC_DataAlign=ADC_DataAlign_Right;//对其模式 右对齐
	ADC_Initstructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None ;//出发控制的触发源   不需要外部触发
	ADC_Initstructure.ADC_ContinuousConvMode=DISABLE;//连续模式 enable 连续   disable  单次
	ADC_Initstructure.ADC_ScanConvMode=DISABLE;//扫描模式 enable扫描模式         disable非扫描
	ADC_Initstructure.ADC_NbrOfChannel=1;//指定规则组通道的数目
 
	ADC_Init(ADC1,&ADC_Initstructure);
 
	ADC_Cmd(ADC1,ENABLE);//ADC上电
 
	//校准
	ADC_ResetCalibration(ADC1);//复位校准
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//返回复位校准的状态  如果没完成就一直循环 复位后会清零
	ADC_StartCalibration(ADC1);//开始校准
	while(ADC_GetCalibrationStatus(ADC1)==SET);//校准是否完成，如果没完成就一直循环
}

uint16_t ADC_Getvalue(uint8_t ADC_Channel)//输入通道获取值
{
	//ADC通道配置	输入通道可以获取值
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_55Cycles5);//在序列1写入通道0  时间55.5个ADC时间
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件触发转换
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//获取标志位状态  规则组转换完成标志位   0转换未完成  1	转换完成
	return ADC_GetConversionValue(ADC1);	
}
