#include "stm32f10x.h"                  // Device header

void ACD_Init(void)
{
	//����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//����ADC��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//��������GPIOAʱ��
	
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);//ADC��Ƶ������Ƶ  72/6=12hz
	//����GPIOA
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
 
	//���ýṹ��
	ADC_InitTypeDef ADC_Initstructure;
	ADC_Initstructure.ADC_Mode=ADC_Mode_Independent;//ģʽ���� ����ģʽ
	ADC_Initstructure.ADC_DataAlign=ADC_DataAlign_Right;//����ģʽ �Ҷ���
	ADC_Initstructure.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None ;//�������ƵĴ���Դ   ����Ҫ�ⲿ����
	ADC_Initstructure.ADC_ContinuousConvMode=DISABLE;//����ģʽ enable ����   disable  ����
	ADC_Initstructure.ADC_ScanConvMode=DISABLE;//ɨ��ģʽ enableɨ��ģʽ         disable��ɨ��
	ADC_Initstructure.ADC_NbrOfChannel=1;//ָ��������ͨ������Ŀ
 
	ADC_Init(ADC1,&ADC_Initstructure);
 
	ADC_Cmd(ADC1,ENABLE);//ADC�ϵ�
 
	//У׼
	ADC_ResetCalibration(ADC1);//��λУ׼
	while(ADC_GetResetCalibrationStatus(ADC1)==SET);//���ظ�λУ׼��״̬  ���û��ɾ�һֱѭ�� ��λ�������
	ADC_StartCalibration(ADC1);//��ʼУ׼
	while(ADC_GetCalibrationStatus(ADC1)==SET);//У׼�Ƿ���ɣ����û��ɾ�һֱѭ��
}

uint16_t ADC_Getvalue(uint8_t ADC_Channel)//����ͨ����ȡֵ
{
	//ADCͨ������	����ͨ�����Ի�ȡֵ
	ADC_RegularChannelConfig(ADC1,ADC_Channel,1,ADC_SampleTime_55Cycles5);//������1д��ͨ��0  ʱ��55.5��ADCʱ��
	
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);//�������ת��
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);//��ȡ��־λ״̬  ������ת����ɱ�־λ   0ת��δ���  1	ת�����
	return ADC_GetConversionValue(ADC1);	
}
