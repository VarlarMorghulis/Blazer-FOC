#ifndef __FOC_REMINDER_H__
#define __FOC_REMINDER_H__

#include "common_inc.h"

/*不同音调对应的PSC值*/
typedef enum
{
	/*不响*/
	T_NONE = 0,
	
	/*低调*/
	T_L1 = FOC_FREQ / 262 - 1,
	T_L2 = FOC_FREQ / 296 - 1,
	T_L3 = FOC_FREQ / 330 - 1,
	T_L4 = FOC_FREQ / 349 - 1,
	T_L5 = FOC_FREQ / 392 - 1,
	T_L6 = FOC_FREQ / 440 - 1,
	T_L7 = FOC_FREQ / 494 - 1,
	
	/*中调*/
	T_M1 = FOC_FREQ / 523 - 1,
	T_M2 = FOC_FREQ / 587 - 1,
	T_M3 = FOC_FREQ / 659 - 1,
	T_M4 = FOC_FREQ / 699 - 1,
	T_M5 = FOC_FREQ / 784 - 1,
	T_M6 = FOC_FREQ / 880 - 1,
	T_M7 = FOC_FREQ / 988 - 1,
	
	/*高调*/
	T_H1 = FOC_FREQ / 1048 - 1,
	T_H2 = FOC_FREQ / 1176 - 1,
	T_H3 = FOC_FREQ / 1320 - 1,
	T_H4 = FOC_FREQ / 1480 - 1,
	T_H5 = FOC_FREQ / 1640 - 1,
	T_H6 = FOC_FREQ / 1760 - 1,
	T_H7 = FOC_FREQ / 1976 - 1
}FOC_Reminder_Tone;

typedef struct
{
	/*音调*/
	uint16_t Tone;
	/*持续时间(ms)*/
	uint16_t Duration;
}Note_TypeDef;

#endif