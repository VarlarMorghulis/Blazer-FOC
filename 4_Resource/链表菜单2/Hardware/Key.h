#ifndef __KEY_H
#define __KEY_H

#define TIME 15
#include "stm32f10x.h"                  // Device header

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif
