#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"

/*按键电平读取函数*/
static uint8_t KEY1_READ_PIN(void)
{
	return HAL_GPIO_ReadPin(KEY1_GPIO_Port,KEY1_Pin);
}

static uint8_t KEY2_READ_PIN(void)
{
	return HAL_GPIO_ReadPin(KEY2_GPIO_Port,KEY2_Pin);
}

typedef enum
{
	/*稳定松开*/
	KS_Release,
	/*抖动*/
	KS_Shake,
	/*确认短按*/
	KS_Press,
	/*等待再次按下*/
	KS_WaitPressAgain,
	/*确认再次按下*/
	KS_PressAgain,
	/*确认长按*/
	KS_LongPress,
	KS_Num
}Key_State;

typedef enum
{
	KEY1,
	KEY2,
	KEY_NUM
}Key_List;

typedef enum
{
	/*空事件*/
	KE_Dummy,
	/*单击*/
	KE_ShortPress,
	/*双击*/
	KE_DoublePress,
	/*长按*/
	KE_LongPress
}Key_Event;

typedef struct
{
	/*按键屏蔽 0屏蔽 1不屏蔽*/
	uint8_t Key_Shield;
	/*按键按下计数*/
	uint16_t Key_PressCnt;
	/*按键再次按下计数*/
	uint16_t Key_PressAgainCnt;	
	/*按键等待计数*/
	uint16_t key_WaitPressAgainCnt;
	/*长按计数*/
	uint16_t Key_LongPressCnt;
	/*按键电平捕获*/
	uint8_t (*Read_Pin)(void);
}Key_TypeDef;

void Key_Status_Check(void);
Key_Event Key_GetStatus(uint8_t KEYx);

#endif