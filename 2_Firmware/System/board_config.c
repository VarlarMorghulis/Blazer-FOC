#include "board_config.h"

void Board_Init(void)
{
	/*��ʱ������ʼ��*/
	delay_init(168);
	
	/*ʹ��TIM7�ж�*/
	HAL_TIM_Base_Start_IT(&htim7);
	
	/*CAN1��ʼ��*/
	CAN_Filter_Init();
	
	/*ABZ��������ʱ������*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1);/*A*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_2);/*B*/
	ABZ_Resolution_Init();
	
	/*�����س�ʼ��*/
	/*ʹ��PWMͨ�����*/
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_3);
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	/*����TIM8ͨ��4PWM*/
	//HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
	
	/*ʹ��ADC1ע�������*/
	HAL_ADCEx_InjectedStart(&hadc1);
	/*ʹ��ADC1ע�����ж�*/
	__HAL_ADC_ENABLE_IT(&hadc1,ADC_IT_JEOC);
	
	OLED_Init();
	Menu_Init();
}
