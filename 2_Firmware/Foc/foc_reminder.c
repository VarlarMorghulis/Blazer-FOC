#include "foc_reminder.h"

FOC_Reminder_Tone FOC_Reminder_Tone_t=T_NONE;

/*FOC上电提示结构体初始化*/
FOC_TypeDef FOC_Reminder_t=
{
	.Udc=24.0f,
	.Ud=0.0f,
	.Uq=0.3f,
	.Tpwm=PWM_TIM_PERIOD,
	.Speed=0.0f
};

/*音符结构体数组初始化*/
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
   * @brief  单音符演奏函数
   * @param  psc 音调 duration 持续时间(ms)
   * @retval
   */
void Motor_Beep_PlayNote(uint16_t tone,uint16_t duration)
{
	
}

void FOC_Task_Reminder(void)
{
	static uint16_t i;
	static uint8_t run_flag;
	/*CAN_ID不同,上电后到提示音响起的延时就不同*/
	/*例如CAN_ID=0x02,上电后1+2*0.5=2秒后听到提示音*/
	uint32_t id_cnt=ReceiveMsg_t.ID*20000;
	
	/*上电后等待一段时间*/
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
	
	/*开始播放提示音*/
	else if(run_flag==1)
	{
		/*依次播放各个音符*/
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