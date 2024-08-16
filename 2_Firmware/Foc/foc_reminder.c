#include "foc_reminder.h"

FOC_Reminder_Tone FOC_Reminder_Tone_t=T_NONE;

/*FOC�ϵ���ʾ�ṹ���ʼ��*/
FOC_TypeDef FOC_Reminder_t=
{
	.Udc=1.0f,
	.Ud=0.0f,
	.Uq=0.004f,
	.Tpwm=PWM_TIM_PERIOD,
	.Speed=0.0f
};

/*�����ṹ�������ʼ��*/
Note_TypeDef Note_t[]=
{
	{.Tone=T_L1,.Duration=300},
	{.Tone=T_M1,.Duration=250},
	{.Tone=T_H1,.Duration=250}
};

TaskElement_TypeDef TE_Reminder_t=
{
	.Step=0,
	.Cnt_20kHz=0
};

extern FOC_State FOC_State_t;
extern ReceiveMsg_TypeDef ReceiveMsg_t;

void FOC_Task_Reminder(void)
{
	static uint16_t note_index;

	FOC_StructBind(&FOC_Reminder_t);
	
	/*�ϵ��ȴ�һ��ʱ��*/
	if(TE_Reminder_t.Step==0)
	{
		if(TE_Reminder_t.Cnt_20kHz<=40000)
		{
			TE_Reminder_t.Cnt_20kHz++;
		}
		else 
		{
			TE_Reminder_t.Cnt_20kHz=0;
			TE_Reminder_t.Step=1;
		}
	}
	
	/*��ʼ������ʾ��*/
	else if(TE_Reminder_t.Step==1)
	{
		/*���β��Ÿ�������*/
		if(note_index<sizeof(Note_t)/sizeof(Note_TypeDef))
		{
			if(TE_Reminder_t.Cnt_20kHz==0)
			{
				__HAL_TIM_SET_PRESCALER(&htim1,Note_t[note_index].Tone);
			}
			
			TE_Reminder_t.Cnt_20kHz++;
			
			if(TE_Reminder_t.Cnt_20kHz/20>=Note_t[note_index].Duration)
			{
				TE_Reminder_t.Cnt_20kHz=0;
				note_index++;
			}
			
			Open_Voltageloop(&FOC_Reminder_t,1);
		}

		else 
		{
			TE_Reminder_t.Step=2;
			note_index=0;
			TE_Reminder_t.Cnt_20kHz=0;
			__HAL_TIM_SET_PRESCALER(&htim1,T_NONE);
			Motor_Release();
		}
	}
	/*������ɺ�ȴ�һ��ʱ��*/
	else if(TE_Reminder_t.Step==2)
	{
		/*�ȴ�1s*/
		if(TE_Reminder_t.Cnt_20kHz<=20000)
		{
			TE_Reminder_t.Cnt_20kHz++;
		}
		else 
		{
			TE_Reminder_t.Step=0;
			TE_Reminder_t.Cnt_20kHz=0;
			FOC_State_t=FOC_Wait;
		}
	}
}