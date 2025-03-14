#ifndef __KEY_H__
#define __KEY_H__

#include "main.h"

/*������ƽ��ȡ����*/
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
	/*�ȶ��ɿ�*/
	KS_Release,
	/*����*/
	KS_Shake,
	/*ȷ�϶̰�*/
	KS_Press,
	/*�ȴ��ٴΰ���*/
	KS_WaitPressAgain,
	/*ȷ���ٴΰ���*/
	KS_PressAgain,
	/*ȷ�ϳ���*/
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
	/*���¼�*/
	KE_Dummy,
	/*����*/
	KE_ShortPress,
	/*˫��*/
	KE_DoublePress,
	/*����*/
	KE_LongPress
}Key_Event;

typedef struct
{
	/*�������� 0���� 1������*/
	uint8_t Key_Shield;
	/*�������¼���*/
	uint16_t Key_PressCnt;
	/*�����ٴΰ��¼���*/
	uint16_t Key_PressAgainCnt;	
	/*�����ȴ�����*/
	uint16_t key_WaitPressAgainCnt;
	/*��������*/
	uint16_t Key_LongPressCnt;
	/*������ƽ����*/
	uint8_t (*Read_Pin)(void);
}Key_TypeDef;

void Key_Status_Check(void);
Key_Event Key_GetStatus(uint8_t KEYx);

#endif