#include "foc_reminder.h"

FOC_Reminder_Tone FOC_Reminder_Tone_t=T_NONE;

/*FOC�ϵ���ʾ�ṹ���ʼ��*/
FOC_TypeDef FOC_Reminder_t=
{
	.Udc=24.0f,
	.Ud=0.0f,
	.Uq=0.3f,
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
	.Init_Flag=0,
	.Run_Flag=0,
	.Cnt_20kHz=0
};

extern FOC_State FOC_State_t;
extern ReceiveMsg_TypeDef ReceiveMsg_t;
/**
   * @brief  ���������ຯ��
   * @param  psc ���� duration ����ʱ��(ms)
   * @retval
   */
void Motor_Beep_PlayNote(uint16_t tone,uint16_t duration)
{
	
}

void FOC_Task_Reminder(void)
{
	static uint16_t i;
	static uint8_t run_flag;
	/*CAN_ID��ͬ,�ϵ����ʾ���������ʱ�Ͳ�ͬ*/
	/*����CAN_ID=0x02,�ϵ��1+2*0.5=2���������ʾ��*/
	uint32_t id_cnt=ReceiveMsg_t.ID*20000;
	
	/*�ϵ��ȴ�һ��ʱ��*/
	if(run_flag==0)
	{
		if(TE_Reminder_t.Cnt_20kHz<=20000+id_cnt)
		{
			TE_Reminder_t.Cnt_20kHz++;
		}
		else 
		{
			TE_Reminder_t.Cnt_20kHz=0;
			run_flag=1;
		}
	}
	
	/*��ʼ������ʾ��*/
	else if(run_flag==1)
	{
		/*���β��Ÿ�������*/
		if(i<sizeof(Note_t)/sizeof(Note_TypeDef))
		{
			if(TE_Reminder_t.Cnt_20kHz==0)
			{
				__HAL_TIM_SET_PRESCALER(&htim1,Note_t[i].Tone);
			}
			
			TE_Reminder_t.Cnt_20kHz++;
			
			if(TE_Reminder_t.Cnt_20kHz/20>=Note_t[i].Duration)
			{
				TE_Reminder_t.Cnt_20kHz=0;
				i++;
			}
			
			Open_Voltageloop(&FOC_Reminder_t,1);
		}
		else 
		{
			run_flag=0;
			TE_Reminder_t.Cnt_20kHz=0;
			__HAL_TIM_SET_PRESCALER(&htim1,T_NONE);
			Motor_Release();
			FOC_State_t=FOC_Wait;
		}
	}
}