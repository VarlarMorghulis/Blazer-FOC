#include "led.h"

LED_TypeDef LED;

void LED_SetState(uint8_t mode_or_error,uint8_t blink_num)
{
	LED.mode_or_error = mode_or_error;
	LED.blink_num = blink_num;
}
	
void LED_SetGPIO(void)
{
	LED_R(LED.on_or_off[1]);
	LED_G(LED.on_or_off[0]);
}

void LED_Task(void)
{
	if(LED.mode_or_error_last != LED.mode_or_error ||
	   LED.blink_num_last != LED.blink_num)
	{
		LED.on_or_off[0] = 0;
		LED.on_or_off[1] = 0;
		LED.cnt = 0;
	}
	
	/*模式指示 绿灯*/
	if(LED.mode_or_error == 0)
	{
		if(LED.cnt < 2 * LED.blink_num)
			LED.on_or_off[0] = LED.cnt % 2;
		else if(LED.cnt < 25)
			LED.on_or_off[0] = 0;
		else
			LED.cnt = 0;
	}
	/*错误指示 红灯*/
	else 
	{
		if(LED.cnt < 2 * LED.blink_num)
			LED.on_or_off[1] = LED.cnt % 2;
		else if(LED.cnt < 25)
			LED.on_or_off[1] = 0;
		else
			LED.cnt = 0;
	}
	
	LED.cnt ++;
	LED.mode_or_error_last = LED.mode_or_error;
	LED.blink_num_last = LED.blink_num;
	LED_SetGPIO();
}