#include "board_config.h"

void Board_Init(void)
{
	/*延时函数初始化*/
	delay_init(168);
	
	/*使能TIM7中断*/
	HAL_TIM_Base_Start_IT(&htim7);
	
	/*CAN1初始化*/
	CAN_Filter_Init();
	
	/*ABZ编码器定时器开启*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1);/*A*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_2);/*B*/
	ABZ_Resolution_Init();
	
	/*电机相关初始化*/
	/*使能PWM通道输出*/
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIMEx_OCN_Start(&htim1,TIM_CHANNEL_3);
	
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	/*开启TIM8通道4PWM*/
	//HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);
	
	/*使能ADC1注入组采样*/
	HAL_ADCEx_InjectedStart(&hadc1);
	/*使能ADC1注入组中断*/
	__HAL_ADC_ENABLE_IT(&hadc1,ADC_IT_JEOC);
	
	OLED_Init();
	Menu_Init();
}
