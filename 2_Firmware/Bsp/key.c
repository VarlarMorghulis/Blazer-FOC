#include "key.h"

Key_State Key_State_t[KEY_NUM]=
{
	KS_Release,
	KS_Release
};

Key_State Key_LastState_t[KEY_NUM]=
{
	KS_Release,
	KS_Release
};

Key_State Key_NowState_t[KEY_NUM]=
{
	KS_Release,
	KS_Release
};

Key_Event Key_Event_t[KEY_NUM]=
{
	KE_Dummy,
	KE_Dummy
};

Key_TypeDef Key_t[KEY_NUM]=
{
	{
		.Key_Shield=1,
		.Read_Pin=KEY1_READ_PIN
	},
	{
		.Key_Shield=1,
		.Read_Pin=KEY2_READ_PIN
	}
};

/**
   * @brief  ����״̬��ɨ�躯�� ����50ms
   * @param  ��
   * @retval ��
   */
void Key_Status_Check(void)
{
	uint8_t i=0;
	for(i=0;i<KEY_NUM;i++)
	{
		if(Key_t[i].Key_Shield==0)
			continue;
		switch(Key_State_t[i])
		{
			case KS_Release:
			{
				Key_Event_t[i]=KE_Dummy;
				/*��⵽�͵�ƽ,�Ƚ�������*/
				if(Key_t[i].Read_Pin()==0)
					Key_State_t[i]=KS_Shake;
			}
			break;

			case KS_Shake:
			{
				if(Key_t[i].Read_Pin()==1)
				{
					/*���ɿ�״̬S0����*/
					if(Key_LastState_t[i]==KS_Release)
						Key_State_t[i]=KS_Release;
								
					/*��ȷ�ϰ���S2״̬����*/
					else if(Key_LastState_t[i]==KS_Press)
					{
						Key_t[i].key_WaitPressAgainCnt=0;
						Key_State_t[i]=KS_WaitPressAgain;
					}
					
					/*��ȷ���ٴΰ���S4����*/
					else if(Key_LastState_t[i]==KS_PressAgain)
					{
						Key_Event_t[i]=KE_DoublePress;
						Key_State_t[i]=KS_Release;
					}
					
					/*��ȷ�ϳ���S5״̬����*/
					else if(Key_LastState_t[i]==KS_LongPress)
						Key_State_t[i]=KS_Release;
					else
					{
						//printf("err\r\n");
					}
				}
				else if(Key_t[i].Read_Pin()==0)
				{
					/*��ȷ�ϰ���S2״̬����*/
					if(Key_LastState_t[i]==KS_Press)
						Key_State_t[i]=KS_Press;
					
					/*��ȷ���ٴΰ���S4״̬����*/
					else if(Key_LastState_t[i]==KS_PressAgain)
						Key_State_t[i]=KS_PressAgain;

					/*��ȷ�ϳ���S5״̬����*/
					else if(Key_LastState_t[i]==KS_LongPress)
						Key_State_t[i]=KS_LongPress;

					/*���ɿ�״̬S0����*/
					else if(Key_LastState_t[i]==KS_Release)
					{
						Key_t[i].Key_PressCnt=0;
						Key_State_t[i]=KS_Press;
					}
					
					/*�ӵȴ��ٴΰ���S3����*/
					else if(Key_LastState_t[i]==KS_WaitPressAgain)
					{
						Key_t[i].Key_PressAgainCnt=0;
						Key_State_t[i]=KS_PressAgain;
					}
					else
					{
						//printf("err\r\n");
					}
				}
			}
			break;

			case KS_Press:
			{
				/*��⵽�ߵ�ƽ,�Ƚ�������*/
				if(Key_t[i].Read_Pin()==1)
					Key_State_t[i]=KS_Shake;
				
				else
				{
					Key_t[i].Key_LongPressCnt++;
					if(Key_t[i].Key_LongPressCnt==30)
					{
						Key_t[i].Key_LongPressCnt=0;
						Key_Event_t[i]=KE_LongPress;
						Key_State_t[i]=KS_LongPress;
					}	
				}
			}
			break;

			case KS_WaitPressAgain:
			{
				/*��⵽�͵�ƽ,�Ƚ�������*/
				if(Key_t[i].Read_Pin()==0)
					Key_State_t[i]=KS_Shake;
				
				else
				{
					Key_t[i].key_WaitPressAgainCnt++;
					if(Key_t[i].key_WaitPressAgainCnt==3)
					{
						Key_Event_t[i]=KE_ShortPress;
						Key_State_t[i]=KS_Release;
					}
				}
			}
			break;

			case KS_PressAgain:
			{
				/*��⵽�ߵ�ƽ,�Ƚ�������*/
				if(Key_t[i].Read_Pin()==1)
					Key_State_t[i]=KS_Shake;
				
				else
				{
					Key_t[i].Key_PressAgainCnt++;
					if(Key_t[i].Key_PressAgainCnt==20)
					{
						Key_t[i].Key_LongPressCnt=0;
						Key_Event_t[i]=KE_LongPress;
						Key_State_t[i]=KS_LongPress;
					}
				}
			}
			break;

			case KS_LongPress:
			{
				Key_Event_t[i]=KE_Dummy;
				
				/*��⵽�ߵ�ƽ,�Ƚ�������*/
				if(Key_t[i].Read_Pin()==1)
					Key_State_t[i]=KS_Shake;
				
				Key_t[i].Key_LongPressCnt++;
			}
			break;

			default:break;	
		}
		
		if(Key_NowState_t[i]!=Key_State_t[i])
		{
			Key_LastState_t[i]=Key_NowState_t[i];
			Key_NowState_t[i]=Key_State_t[i];
		}
	}
}

/**
   * @brief  �����¼���⺯��
   * @param  KEYx����ID
   * @retval �����¼�
   */
Key_Event Key_GetStatus(uint8_t KEYx)
{
	Key_Event key_event;
	
	key_event=Key_Event_t[KEYx];
	Key_Event_t[KEYx]=KE_Dummy;
	
    return key_event;
}
