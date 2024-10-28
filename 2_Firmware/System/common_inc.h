#ifndef __COMMON_INC_H__
#define __COMMON_INC_H__

#include "main.h"

/*Lib*/
#include <arm_math.h> 
#include <arm_const_structs.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*Core*/
#include "gpio.h"
#include "adc.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

/*Bsp*/
#include "delay.h"
#include "led.h"
#include "key.h"
#include "encoder.h"
#include "analog.h"
#include "oled.h"
#include "flash.h"
#include "bsp_task.h"

/*Foc*/
#include "foc_algorithm.h"
#include "foc_calibration.h"
#include "foc_sensorless.h"
#include "foc_identification.h"
#include "foc_param.h"
#include "foc_pid.h"
#include "foc_reminder.h"
#include "foc_utils.h"
#include "foc_run.h"
#include "foc_task.h"

/*Communication*/
#include "interface_can.h"
#include "interface_uart.h"

/*System*/
#include "menu.h"
#include "board_config.h"

#endif
