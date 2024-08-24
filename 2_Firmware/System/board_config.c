#include "board_config.h"

void Board_Init(void)
{
	/*从Flash读取存储的电机和编码器数据*/
	Flash_Read();
	/*从Flash读取CAN_ID*/
	Flash_CAN_ID_Read();
	
	/*延时函数初始化*/
	delay_init(168);
		
	/*ABZ编码器定时器开启*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_1);/*A*/
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_2);/*B*/
	ABZ_Resolution_Init();
	
	Encoder_ParamInit();
	
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
		
	/*使能TIM7中断*/
	HAL_TIM_Base_Start_IT(&htim7);
	
	OLED_Init();
	Menu_Init();
	
	/*CAN1过滤器*/
	CAN_Filter_Init();
	
	/*CAN1初始化*/
	FOC_Param_Init();

}
