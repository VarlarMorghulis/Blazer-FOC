#include "foc_reminder.h"

FOC_Reminder_Tone FOC_Reminder_Tone_t=T_NONE;

/*音符结构体数组初始化*/
Note_TypeDef Note_t[]=
{
	{.Tone=T_L1,.Duration=300},
	{.Tone=T_M1,.Duration=250},
	{.Tone=T_H1,.Duration=250}
};

