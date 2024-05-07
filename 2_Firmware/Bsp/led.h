#ifndef __LED_H__
#define __LED_H__

#include "main.h"

//LED亮灭带参宏 0 熄灭 1点亮
#define LED_R(a)	if(a) \
							LED_R_GPIO_Port->BSRR=(uint32_t)LED_R_Pin<<16U; \
					else  \
							LED_R_GPIO_Port->BSRR=LED_R_Pin;
#define LED_G(a)	if(a) \
							LED_G_GPIO_Port->BSRR=(uint32_t)LED_G_Pin<<16U; \
					else  \
							LED_G_GPIO_Port->BSRR=LED_G_Pin;

//LED电平翻转宏定义
#define LED_R_TOGGLE 	{LED_R_GPIO_Port->ODR^=LED_R_Pin;}	
#define LED_G_TOGGLE 	{LED_G_GPIO_Port->ODR^=LED_G_Pin;}	

void LED_Normal(void);
void LED_Error(void);
void LED_Calib(void);
#endif