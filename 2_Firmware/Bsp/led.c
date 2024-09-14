#include "led.h"

void LED_Normal(void)
{
	LED_G(0);
	LED_R(0);
}

void LED_Error(void)
{
	LED_R_TOGGLE;
	LED_G(0);
}

void LED_Calib(void)
{
	LED_G_TOGGLE;
	LED_R(0);
}