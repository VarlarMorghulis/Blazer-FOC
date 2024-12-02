/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY2_Pin GPIO_PIN_13
#define KEY2_GPIO_Port GPIOC
#define KEY1_Pin GPIO_PIN_14
#define KEY1_GPIO_Port GPIOC
#define TEMP_Pin GPIO_PIN_0
#define TEMP_GPIO_Port GPIOC
#define ENC_CS_Pin GPIO_PIN_1
#define ENC_CS_GPIO_Port GPIOC
#define IC_Pin GPIO_PIN_0
#define IC_GPIO_Port GPIOA
#define IB_Pin GPIO_PIN_1
#define IB_GPIO_Port GPIOA
#define IA_Pin GPIO_PIN_2
#define IA_GPIO_Port GPIOA
#define VBUS_Pin GPIO_PIN_3
#define VBUS_GPIO_Port GPIOA
#define BL_Pin GPIO_PIN_0
#define BL_GPIO_Port GPIOB
#define CL_Pin GPIO_PIN_1
#define CL_GPIO_Port GPIOB
#define AL_Pin GPIO_PIN_13
#define AL_GPIO_Port GPIOB
#define ENC_A_Pin GPIO_PIN_6
#define ENC_A_GPIO_Port GPIOC
#define ENC_B_Pin GPIO_PIN_7
#define ENC_B_GPIO_Port GPIOC
#define ENC_Z_Pin GPIO_PIN_8
#define ENC_Z_GPIO_Port GPIOC
#define ENC_Z_EXTI_IRQn EXTI9_5_IRQn
#define AH_Pin GPIO_PIN_8
#define AH_GPIO_Port GPIOA
#define BH_Pin GPIO_PIN_9
#define BH_GPIO_Port GPIOA
#define CH_Pin GPIO_PIN_10
#define CH_GPIO_Port GPIOA
#define RGB_TIM_Pin GPIO_PIN_15
#define RGB_TIM_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOC
#define OLED_RES_Pin GPIO_PIN_12
#define OLED_RES_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_2
#define OLED_CS_GPIO_Port GPIOD
#define OLED_BLK_Pin GPIO_PIN_3
#define OLED_BLK_GPIO_Port GPIOB
#define LED_G_Pin GPIO_PIN_6
#define LED_G_GPIO_Port GPIOB
#define LED_R_Pin GPIO_PIN_7
#define LED_R_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
