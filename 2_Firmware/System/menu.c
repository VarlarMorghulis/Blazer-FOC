#include "menu.h"
#include "pic.h"

MENU* nowMenu;
uint8_t key1_event,key2_event;
FOC_State FOC_LastState_t;

extern u8g2_t u8g2;
extern uint8_t menu_key;

extern AnalogParam_TypeDef AnalogParam_t;
extern FOC_TypeDef FOC_Sensored_t;
extern ReceiveMsg_TypeDef ReceiveMsg_t;
extern InterfaceParam_TypeDef InterfaceParam_t;
extern Encoder_TypeDef SPI_Encoder_t;
extern Encoder_TypeDef ABZ_Enc_t;
extern PID_TypeDef PID_Iq;
extern PID_TypeDef PID_Speed;
extern PID_TypeDef PID_ZeroSpeed;
extern uint8_t flashsave_flag;

extern FOC_State FOC_State_t;

enum {NoKEY, LEFT, RIGHT, UP, DOWN, ENTER, QUIT};

typedef void (*Show_FunctionPtr)(int8_t, int8_t);
void Show_Function(Show_FunctionPtr Function, int8_t a, int8_t b)
{
    Function(a,b);
}

void Menu_Init(void)
{
    nowMenu = Creat_Menu("MENU",Draw_Main_font,NoFun);
        nowMenu = Creat_ChildMenu("calib",Draw_Calib_font,Calib);
		nowMenu = Creat_BrotherMenu("setting",Draw_Setting_font,Setting);
			nowMenu = Creat_ChildMenu("can id",Draw_CAN_ID_font,CAN_ID);
			nowMenu = Creat_BrotherMenu("i max",Draw_Current_font,Current);
			nowMenu = Creat_BrotherMenu("speed max",Draw_Speed_font,Speed);
			nowMenu = Creat_BrotherMenu("encoder",Draw_Encoder_font,Encoder);
			nowMenu = Creat_BrotherMenu("default",Draw_Default_font,Default);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu("run",Draw_Run_font,Run);
		nowMenu = Creat_BrotherMenu("info",Draw_Info_font,Info);
        nowMenu = Circle_Menu();
    nowMenu = Circle_Menu();
	
	Draw_Main_font(0,0);
}

MENU* Creat_Menu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strncpy(node->Name, Name, 19);
    node->Name[19] = '\0';
    node->Show = Show;
    node->Function = Function;
	node->flag = 0;
	
    node->last = NULL;
    node->next = NULL;
    node->child = node;
    node->parent = node;
	
    return node;
}

MENU* Creat_ChildMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strncpy(node->Name, Name, 19);
    node->Name[19] = '\0';
    node->Show = Show;
    node->Function = Function;
	node->flag = 0;
	
    node->last = NULL;
    node->next = NULL;
    node->child = node;
    node->parent = nowMenu;

    nowMenu->child = node;
	
    return node;
}

MENU* Creat_BrotherMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)())
{
    MENU* node = malloc(sizeof(MENU));
    strncpy(node->Name, Name, 19);
    node->Name[19] = '\0';
    node->Show = Show;
    node->Function = Function;
	node->flag = 0;
	
    node->last = nowMenu;
    node->next = NULL;
    node->child = node;
    node->parent = nowMenu->parent;

    nowMenu->next = node;

    return node;
}

MENU* Circle_Menu(void)
{
    MENU* finalMenu = nowMenu;
    while(nowMenu->last != NULL)
    {
        nowMenu = nowMenu->last;
    }
    nowMenu->last = finalMenu;
    finalMenu->next = nowMenu;

    return nowMenu->parent;
}

uint8_t Menu_GetKeyval(void)
{
	uint8_t KeyVal = NoKEY;
	
	key1_event=Key_GetStatus(KEY1);
	key2_event=Key_GetStatus(KEY2);
	
	if(key1_event==KE_LongPress)
	{
		if(nowMenu->child!=nowMenu)	
			KeyVal=DOWN;
		else
			KeyVal=ENTER;
	}
	else if(key1_event==KE_ShortPress)
	{	
		KeyVal=RIGHT;
	}
	
	if(key2_event==KE_LongPress)
	{
		if(nowMenu->flag==1)
			KeyVal=QUIT;
		else
			KeyVal=UP;
	}
	else if(key2_event==KE_ShortPress)
	{
		KeyVal=LEFT;
	}
	
	return KeyVal;
}

void Choose_Menu(void)
{
	/*还未进入具体的某个功能页面*/
	if(nowMenu->flag==0)
	{
		switch(menu_key)
		{
			case NoKEY:
				return;
			
			case DOWN:
				nowMenu = nowMenu->child;
			break;
			
			case UP:
				nowMenu = nowMenu->parent;
			break;
			
			case LEFT:
				nowMenu = nowMenu->last;
			break;
			
			case RIGHT:
				nowMenu = nowMenu->next;
			break;
	        
			case ENTER:
			{
				nowMenu->flag=1;
				/*进入页面后清空按键状态,防止相同的操作二次触发*/
				key1_event=KE_Dummy;
				key2_event=KE_Dummy;
			}
			break;
			
			default:break;
		}
	}
	/*当前在某个功能页面里*/
	else if(nowMenu->flag==1)
	{
		switch(menu_key)
		{
			case QUIT:
				nowMenu->flag=0;
			break;
			
			default:break;
		}
	}
		
	if(menu_key!=NoKEY || FOC_State_t!=FOC_LastState_t)
    {
		Menu_Show(menu_key);
	}
	
	FOC_LastState_t=FOC_State_t;
}

void Menu_Show(uint8_t key)
{
	uint8_t i;
	MENU* now = nowMenu;
	
	if(nowMenu->flag==0)
	{
		for(i=0;i<8;i++)
		{
			if(key==LEFT)
			{
				u8g2_ClearBuffer(&u8g2);
				Show_Function(nowMenu->Show, -28+i*8, 0);
				nowMenu = nowMenu->next;
				Show_Function(nowMenu->Show, 40+i*8, 0);
				nowMenu = nowMenu->next;
				Show_Function(nowMenu->Show, 108+i*8, 0);
				nowMenu = now->last;
				Show_Function(nowMenu->Show, -96+i*8, 0);
				u8g2_SendBuffer(&u8g2);		
				delay_ms(5);
			}
			else if(key==RIGHT)
			{
				u8g2_ClearBuffer(&u8g2);
				Show_Function(nowMenu->Show, 108-i*8, 0);
				nowMenu = nowMenu->last;
				Show_Function(nowMenu->Show, 40-i*8, 0);
				nowMenu = nowMenu->last;
				Show_Function(nowMenu->Show, -28-i*8, 0);
				nowMenu = now->next;
				Show_Function(nowMenu->Show, 176-i*8, 0);
				u8g2_SendBuffer(&u8g2);
				delay_ms(5);
			}
			else if(key==DOWN)
			{
//				u8g2_ClearBuffer(&u8g2);
//				nowMenu=now;
//				Show_Function(nowMenu->Show, 0, 40);
//				nowMenu = now->last;
//				Show_Function(nowMenu->Show, 0, 40-i*8);
//				nowMenu = now->next;
//				Show_Function(nowMenu->Show, 0, 40+i*8);
//				u8g2_SendBuffer(&u8g2);
//				delay_ms(5);
			}
			else if(key==UP)
			{
//				u8g2_ClearBuffer(&u8g2);
//				nowMenu=now;
//				Show_Function(nowMenu->Show, 8-i, 40);
//				nowMenu = now->last;
//				Show_Function(nowMenu->Show, 8-i, 40-i*8);
//				nowMenu = now->next;
//				Show_Function(nowMenu->Show, 8-i, 40+i*8);
//				u8g2_SendBuffer(&u8g2);
//				delay_ms(5);		
			}
			
			nowMenu = now;
		}
	}
	
	u8g2_ClearBuffer(&u8g2);
	
	if(nowMenu->flag==1)
	{
		(*nowMenu->Function)();
	}
	else
	{
		NoFun();
		Show_Function(nowMenu->Show, 0, 0);
		nowMenu = now->last;
		Show_Function(nowMenu->Show, -65, 0);
		nowMenu = now->next;
		Show_Function(nowMenu->Show, 65, 0);
	}
	
	nowMenu = now;
	
	if(nowMenu->parent!=nowMenu&&nowMenu->flag!=1)
	{
		u8g2_DrawTriangle(&u8g2,99,40,94,32,94,48);
		u8g2_DrawTriangle(&u8g2,29,40,34,32,34,48);
	}
	
	u8g2_SendBuffer(&u8g2);
	
    nowMenu = now;
}


void Draw_Main_font(int8_t x, int8_t y)
{
	char ID_str[10],Vbus_str[10];
	
	u8g2_ClearBuffer(&u8g2);
	
	u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tf);
	
	sprintf(ID_str,"%d",ReceiveMsg_t.NodeID);
	u8g2_DrawStr(&u8g2, 0, 18, "0x0");
    u8g2_DrawStr(&u8g2, 35, 18, ID_str);
		
	//sprintf(Vbus_str,"%.1f",AnalogParam_t.vbus);
	sprintf(Vbus_str,"%-.1fV",AnalogParam_t.vbus);
    u8g2_DrawStr(&u8g2, 65, 18, Vbus_str);
	//u8g2_DrawStr(&u8g2, 105, 18, "V");
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
	
	u8g2_DrawStr(&u8g2,10,40,"Motor");
	
	u8g2_DrawStr(&u8g2,10,55,"Encoder");
	
	u8g2_DrawStr(&u8g2,10,70,"Resolution");
	
#ifdef USE_ABZ_ENCODER
	u8g2_DrawStr(&u8g2,80,55,"ABZ");
	sprintf(Resolution_str,"%d",ABZ_Enc_t.cpr);
	u8g2_DrawStr(&u8g2,80,70,Resolution_str);
#endif

#ifdef USE_SPI_ENCODER
	switch(SPI_Encoder_t.enc_type)
	{
		case TLE5012B:
			u8g2_DrawStr(&u8g2,80,55,"TLE5012B");
		break;
		
		case MT6816:
			u8g2_DrawStr(&u8g2,80,55,"MT6816");
		break;
		
		default:
			u8g2_DrawStr(&u8g2,80,55,"Unknown");
		break;
	}
	
#endif

#ifdef Motor_Tmotor_U10
	u8g2_DrawStr(&u8g2,80,40,"U10");
#endif

#ifdef Motor_5065
	u8g2_DrawStr(&u8g2,80,40,"5065");
#endif

	u8g2_SendBuffer(&u8g2);
}

void Draw_Calib_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_calib);
	u8g2_DrawStr(&u8g2,50+x,70,nowMenu->Name);
}

void Draw_Setting_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_setting);
	u8g2_DrawStr(&u8g2,44+x,70,nowMenu->Name);
}

void Draw_CAN_ID_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_can_id);
	u8g2_DrawStr(&u8g2,46+x,70,nowMenu->Name);
}

void Draw_Current_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_current);
	u8g2_DrawStr(&u8g2,48+x,70,nowMenu->Name);
}

void Draw_Speed_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_speed);
	u8g2_DrawStr(&u8g2,38+x,70,nowMenu->Name);
}

void Draw_Encoder_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_encoder);
	u8g2_DrawStr(&u8g2,44+x,70,nowMenu->Name);
}

void Draw_Default_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_default);
	u8g2_DrawStr(&u8g2,44+x,70,nowMenu->Name);
}

void Draw_Run_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_run);
	u8g2_DrawStr(&u8g2,55+x,70,nowMenu->Name);
}

void Draw_Info_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44+x,16,40,40,gImage_info);
	u8g2_DrawStr(&u8g2,50+x,70,nowMenu->Name);
}

void NoFun(void)
{
	FOC_State_t=FOC_Wait;
}

void Calib(void)
{
	static uint8_t step=0;
	
	if(step==0)
	{
		FOC_State_t=FOC_Wait;
	}
	if(key1_event==KE_ShortPress)
	{
		FOC_State_t=FOC_Calibration;
		step=1;
	}
	else if(step==1)
	{
		if(FOC_State_t==FOC_Wait)
			step=2;
		else if(FOC_State_t==FOC_Error)
			step=3;
	}

	if(step==0)
	{
		u8g2_DrawStr(&u8g2,0,20,"Please set the");
		u8g2_DrawStr(&u8g2,0,40,"motor free be-");
		u8g2_DrawStr(&u8g2,0,60,"fore start");
		u8g2_DrawTriangle(&u8g2,120,15,115,10,115,20);
		
	}
	else if(step==1)
	{
		u8g2_SetFont(&u8g2,u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2,20,40,"Calibrating...");
		u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
	}
	else if(step==2)
	{
		u8g2_SetFont(&u8g2,u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2,40,40,"Succeed!");
		u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
		u8g2_DrawTriangle(&u8g2,110,70,105,65,105,75);
		u8g2_DrawTriangle(&u8g2,120,70,115,65,115,75);
		step=0;
	}
	else if(step==3)
	{
		u8g2_SetFont(&u8g2,u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2,40,40,"Failed!");
		u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
		step=0;
	}
}

void Setting(void)
{
	u8g2_DrawStr(&u8g2,20,35,"To be developed");
}


void CAN_ID(void)
{
	static uint8_t step=0;
	char can_id_str[10];

	if(step==0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event==KE_LongPress)
		{
			key1_event=KE_Dummy;
			step=1;
		}
	}
	if(step==1)
	{
		/*ID增加*/
		if(key1_event==KE_ShortPress)
		{
			ReceiveMsg_t.NodeID++;
			if(ReceiveMsg_t.NodeID>7)
				ReceiveMsg_t.NodeID=0;
		}
		/*ID减小*/
		if(key2_event==KE_ShortPress)
		{
			ReceiveMsg_t.NodeID--;
			if((int8_t)ReceiveMsg_t.NodeID<0)
				ReceiveMsg_t.NodeID=7;
		}
		
		/*保存配置*/
		if(key1_event==KE_LongPress)
		{
			InterfaceParam_t.node_id=(float)ReceiveMsg_t.NodeID;
			flashsave_flag=1;
			step=0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step==1)
	{
		u8g2_DrawTriangle(&u8g2,96,40,91,32,91,48);
		u8g2_DrawTriangle(&u8g2,25,40,31,32,31,48);
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tf);
	
	sprintf(can_id_str,"0x0%d",ReceiveMsg_t.NodeID);
	u8g2_DrawStr(&u8g2,39,50,can_id_str);
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
}

void Current(void)
{
	static uint8_t step=0;
	char current_str[10];

	if(step==0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event==KE_LongPress)
		{
			key1_event=KE_Dummy;
			step=1;
		}
	}
	if(step==1)
	{
		/*设定电流增加*/
		if(key1_event==KE_ShortPress)
		{
			PID_Speed.output_max+=1.0f;
			if(PID_Speed.output_max>70.0f)
				PID_Speed.output_max=5.0f;
		}
		/*设定电流减小*/
		if(key2_event==KE_ShortPress)
		{
			PID_Speed.output_max-=1.0f;
			if(PID_Speed.output_max<5.0f)
				PID_Speed.output_max=70.0f;
		}
		
		/*长按保存配置*/
		if(key1_event==KE_LongPress)
		{
			PID_ZeroSpeed.output_max=PID_Speed.output_max;
			InterfaceParam_t.current_max=PID_Speed.output_max;
			flashsave_flag=1;
			step=0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step==1)
	{
		u8g2_DrawTriangle(&u8g2,96,40,91,32,91,48);
		u8g2_DrawTriangle(&u8g2,25,40,31,32,31,48);
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tf);
	
	sprintf(current_str,"%dA",(int)PID_Speed.output_max);
	u8g2_DrawStr(&u8g2,44,46,current_str);
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
}

void Speed(void)
{
	u8g2_DrawStr(&u8g2,20,35,"To be developed");
}

void Encoder(void)
{
	static uint8_t step=0;
	char default_str[10];
    
	if(step==0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event==KE_LongPress)
		{
			key1_event=KE_Dummy;
			step=1;
		}
	}
	if(step==1)
	{
		if(key1_event==KE_ShortPress)
		{
			SPI_Encoder_t.enc_type++;
			if(SPI_Encoder_t.enc_type>1)
				SPI_Encoder_t.enc_type=TLE5012B;
		}
		if(key2_event==KE_ShortPress)
		{
			SPI_Encoder_t.enc_type--;
			if((int8_t)SPI_Encoder_t.enc_type<0)
				SPI_Encoder_t.enc_type=MT6816;
		}
		
		/*保存配置*/
		if(key1_event==KE_LongPress)
		{
			InterfaceParam_t.enc_type=(float)SPI_Encoder_t.enc_type;
			flashsave_flag=1;
			step=0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step==1)
	{
		u8g2_DrawTriangle(&u8g2,106,40,101,32,101,48);
		u8g2_DrawTriangle(&u8g2,15,40,21,32,21,48);
	}
	
	//u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tf);
	
	switch(SPI_Encoder_t.enc_type)
	{
		case TLE5012B:
			sprintf(default_str,"TLE5012B");
			u8g2_DrawStr(&u8g2,38,46,default_str);
		break;
		
		case MT6816:
			sprintf(default_str,"MT6816");
			u8g2_DrawStr(&u8g2,45,46,default_str);
		break;
		
		default:break;
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
}

void Default(void)
{
	static uint8_t step=0;
	static uint8_t if_return_default;
	char default_str[10];
    
	if(step==0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event==KE_LongPress)
		{
			key1_event=KE_Dummy;
			step=1;
		}
	}
	if(step==1)
	{
		if(key1_event==KE_ShortPress)
		{
			if_return_default++;
			if(if_return_default>1)
				if_return_default=0;
		}
		if(key2_event==KE_ShortPress)
		{
			if_return_default--;
			if((int8_t)if_return_default<0)
				if_return_default=1;
		}
		
		/*保存配置*/
		if(key1_event==KE_LongPress && if_return_default==1)
		{
			Param_Return_Default();
			flashsave_flag=1;
			step=0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step==1)
	{
		u8g2_DrawTriangle(&u8g2,96,40,91,32,91,48);
		u8g2_DrawTriangle(&u8g2,25,40,31,32,31,48);
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_ncenB14_tf);
	
	if(if_return_default==0)
	{
		sprintf(default_str,"No");
	}
	else if(if_return_default==1)
	{
		sprintf(default_str,"Yes");
	}
	
	u8g2_DrawStr(&u8g2,44,46,default_str);
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
}

void Run(void)
{
	char Iq_ref_str[10],Iq_now_str[10];
	static uint8_t step;
	if(step==0)
	{
		FOC_State_t=FOC_Sensored;
		step=1;
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_ncenB10_tf);
	u8g2_DrawStr(&u8g2,10,20,"Current");
	u8g2_DrawStr(&u8g2,28,36,"Mode");
	
	if(key1_event==KE_ShortPress)
	{
		//PID_Iq.ref_value+=1.0f;
		PID_Speed.ref_value+=_2PI;
	}
	else if(key2_event==KE_ShortPress)
	{
		//PID_Iq.ref_value-=1.0f;
		PID_Speed.ref_value-=_2PI;
	}
	
	u8g2_SetFont(&u8g2,u8g2_font_6x10_mf);
	
	sprintf(Iq_ref_str,"Iq_ref  %3.2fA",PID_Iq.ref_value);
	u8g2_DrawStr(&u8g2,10,50,Iq_ref_str);
	
	sprintf(Iq_now_str,"Iq_now  %3.2fA",PID_Iq.samp_value);
	u8g2_DrawStr(&u8g2,10,62,Iq_now_str);
}

void Info(void)
{
	u8g2_DrawStr(&u8g2,20,35,"To be developed");
}
