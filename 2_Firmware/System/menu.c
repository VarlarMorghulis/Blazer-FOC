#include "menu.h"
#include "pic.h"

MENU* nowMenu;
uint8_t key1_event,key2_event;
ModeNow_TypeDef Menu_ModeLast = Motor_Disable;

extern u8g2_t u8g2;
extern uint8_t menu_key;

extern MotorControl_TypeDef MotorControl;
extern Encoder_TypeDef Encoder;
extern CANMsg_TypeDef CANMsg;

enum {NoKEY, LEFT, RIGHT, UP, DOWN, ENTER, QUIT};

typedef void (*Show_FunctionPtr)(int8_t, int8_t);
void Show_Function(Show_FunctionPtr Function, int8_t a, int8_t b)
{
    Function(a,b);
}

void Menu_Init(void)
{
    nowMenu = Creat_Menu("MENU", Draw_Main_font,Main);
        nowMenu = Creat_ChildMenu("calib", Draw_Calib_font, Calib);
			nowMenu = Creat_ChildMenu("r l flux", Draw_R_L_Flux_font, R_L_Flux);
			nowMenu = Creat_BrotherMenu("offset", Draw_Encoder_Offset_font, Encoder_Offset);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu("setting", Draw_Setting_font, Setting);
			nowMenu = Creat_ChildMenu("can id", Draw_CAN_ID_font, CAN_ID);
			nowMenu = Creat_BrotherMenu("i limit", Draw_Current_font, Current);
			nowMenu = Creat_BrotherMenu("speed limit", Draw_Speed_font, Speed);
			nowMenu = Creat_BrotherMenu("encoder", Draw_Encoder_font, EncoderType);
			nowMenu = Creat_BrotherMenu("pole pairs", Draw_Pole_Pairs_font, Pole_Pairs);
			nowMenu = Creat_BrotherMenu("default", Draw_Default_font, Default);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu("run", Draw_Run_font,Run);
		nowMenu = Creat_BrotherMenu("info", Draw_Info_font,Info);
        nowMenu = Circle_Menu();
    nowMenu = Circle_Menu();
	
	Draw_Main_font(0, 0);
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
	
	key1_event = Key_GetStatus(KEY1);
	key2_event = Key_GetStatus(KEY2);
	
	if(key1_event == KE_LongPress)
	{
		if(nowMenu->child != nowMenu)	
			KeyVal = DOWN;
		else
			KeyVal = ENTER;
	}
	else if(key1_event == KE_ShortPress)
	{	
		KeyVal = RIGHT;
	}
	
	if(key2_event == KE_LongPress)
	{
		if(nowMenu->flag == 1)
			KeyVal = QUIT;
		else
			KeyVal = UP;
	}
	else if(key2_event == KE_ShortPress)
	{
		KeyVal = LEFT;
	}
	
	return KeyVal;
}

bool Main_ValueChange(void)
{
	static bool is_init  = false;
	bool is_value_change = false;
	bool is_in_main = (nowMenu->Name[0] == 'M' && nowMenu->Name[1] == 'E' && nowMenu->Name[2] == 'N' && nowMenu->Name[3] == 'U');
	static uint8_t NodeID_last;
	static float vbus_last;
	static int32_t pole_pairs_last;
	static Encoder_Type encoder_type_last;
	
	if(is_init == true)
	{	
		if(is_in_main == true)
		{
			if(NodeID_last != CANMsg.node_id)
			{
				is_value_change = true;
			}
			else if((int)(10.0f * vbus_last) != (int)(10.0f * MotorControl.vbus))
			{
				is_value_change = true;
			}
			else if(pole_pairs_last != MotorControl.motor_pole_pairs)
			{
				is_value_change = true;
			}
			else if(encoder_type_last != Encoder.encoder_type)
			{
				Encoder_ParamInit();
				is_value_change = true;
			}
			else
			{
				is_value_change = false;
			}
		}
		else
		{
			is_value_change = false;
		}
	}
	
	if(is_init == false)
		is_init = true;
	
	NodeID_last = CANMsg.node_id;
	vbus_last = MotorControl.vbus;
	pole_pairs_last = MotorControl.motor_pole_pairs;
	encoder_type_last = Encoder.encoder_type;
	
	return is_value_change;
}
void Choose_Menu(void)
{
	bool is_value_change = false;
	
	is_value_change = Main_ValueChange();
	
	/*还未进入具体的某个功能页面*/
	if(nowMenu->flag == 0)
	{
		switch(menu_key)
		{
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
				nowMenu->flag = 1;
				/*进入页面后清空按键状态,防止相同的操作二次触发*/
				key1_event = KE_Dummy;
				key2_event = KE_Dummy;
			}
			break;
			
			case NoKEY:
				if(is_value_change == false)
					return;
			default:break;
		}
	}
	/*当前在某个功能页面里*/
	else if(nowMenu->flag == 1)
	{
		switch(menu_key)
		{
			case QUIT:
				nowMenu->flag = 0;
			break;
			
			default:break;
		}
	}
	
	
	if(menu_key != NoKEY || Menu_ModeLast != MotorControl.ModeNow || is_value_change == true)
    {
		Menu_Show(menu_key);
	}
	
	Menu_ModeLast = MotorControl.ModeNow;
}

void Menu_Show(uint8_t key)
{
	MENU* now = nowMenu;
	
	if(nowMenu->flag == 0)
	{
		for(int i=0; i<8; i++)
		{
			if(key == LEFT)
			{
				u8g2_ClearBuffer(&u8g2);
				Show_Function(nowMenu->Show, -28 + i * 8, 0);
				nowMenu = nowMenu->next;
				Show_Function(nowMenu->Show,  40 + i * 8, 0);
				nowMenu = nowMenu->next;
				Show_Function(nowMenu->Show, 108 + i * 8, 0);
				nowMenu = now->last;
				Show_Function(nowMenu->Show, -96 + i * 8, 0);
				u8g2_SendBuffer(&u8g2);		
				delay_ms(5);
			}
			else if(key == RIGHT)
			{
				u8g2_ClearBuffer(&u8g2);
				Show_Function(nowMenu->Show, 108 - i * 8, 0);
				nowMenu = nowMenu->last;
				Show_Function(nowMenu->Show,  40 - i * 8, 0);
				nowMenu = nowMenu->last;
				Show_Function(nowMenu->Show, -28 - i * 8, 0);
				nowMenu = now->next;
				Show_Function(nowMenu->Show, 176 - i * 8, 0);
				u8g2_SendBuffer(&u8g2);
				delay_ms(5);
			}
			else if(key == DOWN)
			{
			}
			else if(key == UP)
			{		
			}
		
			nowMenu = now;
		}
	}
	
	u8g2_ClearBuffer(&u8g2);
	
	if(nowMenu->flag == 1)
	{
		(*nowMenu->Function)();
	}
	else
	{
		Show_Function(nowMenu->Show,   0, 0);
		nowMenu = now->last;
		Show_Function(nowMenu->Show, -65, 0);
		nowMenu = now->next;
		Show_Function(nowMenu->Show,  65, 0);
	}
	
	nowMenu = now;
	
	if(nowMenu->parent != nowMenu && nowMenu->flag != 1)
	{
		u8g2_DrawTriangle(&u8g2, 99, 40, 94, 32, 94, 48);
		u8g2_DrawTriangle(&u8g2, 29, 40, 34, 32, 34, 48);
	}
	
	u8g2_SendBuffer(&u8g2);
	
    nowMenu = now;
}


void Draw_Main_font(int8_t x, int8_t y)
{
	char ID_str[10],Vbus_str[10],PP_str[10];
	
	static bool is_init_draw = false;

	if(is_init_draw == false)
		u8g2_ClearBuffer(&u8g2);
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tf);
	
	sprintf(ID_str, "%d", CANMsg.node_id);
	u8g2_DrawStr(&u8g2, 10, 18, "0x0");
    u8g2_DrawStr(&u8g2, 45, 18, ID_str);
		
	sprintf(Vbus_str, "%-.1fV", MotorControl.vbus);
    u8g2_DrawStr(&u8g2, 68, 18, Vbus_str);
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
	
	u8g2_DrawStr(&u8g2, 10, 40, "PolePairs");
	
	u8g2_DrawStr(&u8g2, 10, 55, "Encoder");
	
	u8g2_DrawStr(&u8g2, 10, 70, "Resolution");
	
	sprintf(PP_str, "%d", MotorControl.motor_pole_pairs);
	u8g2_DrawStr(&u8g2, 80, 40, PP_str);
	
	switch(Encoder.encoder_type)
	{
		case TLE5012B:
			u8g2_DrawStr(&u8g2, 80, 55, "TLE5012B");
			u8g2_DrawStr(&u8g2, 80, 70, "32768");
		break;
		
		case MT6816:
			u8g2_DrawStr(&u8g2, 80, 55, "MT6816");
			u8g2_DrawStr(&u8g2, 80, 70, "16384");
		break;
		
		default:
			u8g2_DrawStr(&u8g2, 80, 55, "Unknown");
		break;
	}
	
	if(is_init_draw == false)
		u8g2_SendBuffer(&u8g2);
	
	if(is_init_draw == false)
		is_init_draw = true;
}

void Draw_Calib_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_calib);
	u8g2_DrawStr(&u8g2, 50 + x, 70, nowMenu->Name);
}

void Draw_R_L_Flux_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_r_l_flux);
	u8g2_DrawStr(&u8g2, 40 + x, 70, nowMenu->Name);
}

void Draw_Encoder_Offset_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_encoder_offset);
	u8g2_DrawStr(&u8g2, 45 + x, 70, nowMenu->Name);
}

void Draw_Setting_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_setting);
	u8g2_DrawStr(&u8g2, 44 + x, 70, nowMenu->Name);
}

void Draw_CAN_ID_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_can_id);
	u8g2_DrawStr(&u8g2, 46 + x, 70, nowMenu->Name);
}

void Draw_Current_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2,44 + x, 16, 40, 40, gImage_current);
	u8g2_DrawStr(&u8g2,42 + x, 70, nowMenu->Name);
}

void Draw_Speed_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_speed);
	u8g2_DrawStr(&u8g2, 32 + x, 70, nowMenu->Name);
}

void Draw_Encoder_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_encoder);
	u8g2_DrawStr(&u8g2, 44 + x, 70, nowMenu->Name);
}

void Draw_Pole_Pairs_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_pole_paris);
	u8g2_DrawStr(&u8g2, 35 + x, 70, nowMenu->Name);
}

void Draw_Default_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_default);
	u8g2_DrawStr(&u8g2, 44 + x, 70, nowMenu->Name);
}

void Draw_Run_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_run);
	u8g2_DrawStr(&u8g2, 55 + x, 70, nowMenu->Name);
}

void Draw_Info_font(int8_t x, int8_t y)
{
	u8g2_DrawXBMP(&u8g2, 44 + x, 16, 40, 40, gImage_info);
	u8g2_DrawStr(&u8g2, 50 + x, 70, nowMenu->Name);
}

void Main(void)
{

}

void Calib(void)
{

}

void R_L_Flux(void)
{
	static uint8_t step = 0;
	
	if(step == 0)
	{
		MotorControl.ModeNow = Motor_Disable;
	}
	if(key1_event == KE_ShortPress)
	{
		MotorControl.ModeNow = Calib_Motor_R_L_Flux;
		step = 1;
	}
	else if(step == 1)
	{
		if(MotorControl.ModeNow == Motor_Disable && MotorControl.ErrorNow == No_Error)
			step = 2;
		if(MotorControl.ErrorNow != No_Error)
			step = 3;
	}

	if(step == 0)
	{
		u8g2_DrawStr(&u8g2, 0, 20, "Please set the");
		u8g2_DrawStr(&u8g2, 0, 40, "motor free be-");
		u8g2_DrawStr(&u8g2, 0, 60, "fore start");
		u8g2_DrawTriangle(&u8g2, 120, 15, 115, 10, 115, 20);
		
	}
	else if(step == 1)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 20, 40, "Calibrating...");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
	}
	else if(step == 2)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 40, 40, "Succeed!");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
		u8g2_DrawTriangle(&u8g2, 110, 70, 105, 65, 105, 75);
		u8g2_DrawTriangle(&u8g2, 120, 70, 115, 65, 115, 75);
		step = 0;
	}
	else if(step == 3)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 40, 40,"Failed!");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
		step = 0;
	}
}

void Encoder_Offset(void)
{
	static uint8_t step=0;
	
	if(step == 0)
	{
		MotorControl.ModeNow = Motor_Disable;
	}
	if(key1_event == KE_ShortPress)
	{
		MotorControl.ModeNow = Calib_EncoderOffset;
		step = 1;
	}
	else if(step == 1)
	{
		if(MotorControl.ModeNow == Motor_Disable && MotorControl.ErrorNow == No_Error)
			step = 2;
		if(MotorControl.ErrorNow != No_Error)
			step = 3;
	}

	if(step == 0)
	{
		u8g2_DrawStr(&u8g2, 0, 20, "Please set the");
		u8g2_DrawStr(&u8g2, 0, 40, "motor free be-");
		u8g2_DrawStr(&u8g2, 0, 60, "fore start");
		u8g2_DrawTriangle(&u8g2, 120, 15, 115, 10, 115, 20);
		
	}
	else if(step == 1)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 20, 40, "Calibrating...");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
	}
	else if(step == 2)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 40, 40, "Succeed!");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
		u8g2_DrawTriangle(&u8g2, 110, 70, 105, 65, 105, 75);
		u8g2_DrawTriangle(&u8g2, 120, 70, 115, 65, 115, 75);
		step = 0;
	}
	else if(step == 3)
	{
		u8g2_SetFont(&u8g2, u8g2_font_6x12_mf);
		u8g2_DrawStr(&u8g2, 40, 40,"Failed!");
		u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
		step = 0;
	}
	
}

void Setting(void)
{

}


void CAN_ID(void)
{
	static uint8_t step = 0;
	char can_id_str[10];

	if(step == 0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event == KE_LongPress)
		{
			key1_event = KE_Dummy;
			step = 1;
		}
	}
	if(step == 1)
	{
		/*ID增加*/
		if(key1_event == KE_ShortPress)
		{
			CANMsg.node_id++;
			if(CANMsg.node_id > 7)
				CANMsg.node_id = 0;
		}
		/*ID减小*/
		if(key2_event == KE_ShortPress)
		{
			CANMsg.node_id--;
			if((int8_t)CANMsg.node_id < 0)
				CANMsg.node_id = 7;
		}
		
		/*保存配置*/
		if(key1_event == KE_LongPress)
		{
			MotorControl.ModeNow = Save_Param;
			step = 0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step == 1)
	{
		u8g2_DrawTriangle(&u8g2, 96, 40, 91, 32, 91, 48);
		u8g2_DrawTriangle(&u8g2, 25, 40, 31, 32, 31, 48);
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tf);
	
	sprintf(can_id_str, "0x0%d", CANMsg.node_id);
	u8g2_DrawStr(&u8g2, 39, 50, can_id_str);
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void Current(void)
{
	static uint8_t step = 0;
	char current_str[10];

	if(step == 0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入最大电流配置*/
		if(key1_event == KE_LongPress)
		{
			key1_event = KE_Dummy;
			step = 1;
		}
	}
	if(step == 1)
	{
		/*设定电流增加*/
		if(key1_event == KE_ShortPress)
		{
			MotorControl.current_limit += 1.0f;
			if(MotorControl.current_limit > 60.0f)
				MotorControl.current_limit = 5.0f;
		}
		/*设定电流减小*/
		if(key2_event == KE_ShortPress)
		{
			MotorControl.current_limit -= 1.0f;
			if(MotorControl.current_limit < 5.0f)
				MotorControl.current_limit = 60.0f;
		}
		
		/*长按保存配置*/
		if(key1_event == KE_LongPress)
		{
			MotorControl.ModeNow = Save_Param;
			step = 0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step == 1)
	{
		u8g2_DrawTriangle(&u8g2, 96, 40, 91, 32, 91, 48);
		u8g2_DrawTriangle(&u8g2, 25, 40, 31, 32, 31, 48);
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tf);
	
	sprintf(current_str, "%dA", (int)MotorControl.current_limit);
	u8g2_DrawStr(&u8g2, 44, 46, current_str);
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void Speed(void)
{
	u8g2_DrawStr(&u8g2,20,35,"To be developed");
}

void EncoderType(void)
{
	static uint8_t step = 0;
	char encoder_str[10];
    
	if(step == 0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入编码器型号配置*/
		if(key1_event == KE_LongPress)
		{
			key1_event = KE_Dummy;
			step = 1;
		}
	}
	if(step == 1)
	{
		if(key1_event == KE_ShortPress)
		{
			Encoder.encoder_type++;
			if(Encoder.encoder_type > 1)
				Encoder.encoder_type = TLE5012B;
		}
		if(key2_event == KE_ShortPress)
		{
			Encoder.encoder_type--;
			if((int8_t)Encoder.encoder_type < 0)
				Encoder.encoder_type = MT6816;
		}
		
		/*保存配置*/
		if(key1_event == KE_LongPress)
		{
			MotorControl.ModeNow = Save_Param;
			step = 0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step == 1)
	{
		u8g2_DrawTriangle(&u8g2, 106, 40, 101, 32, 101, 48);
		u8g2_DrawTriangle(&u8g2,  15, 40,  21, 32,  21, 48);
	}
	
	switch(Encoder.encoder_type)
	{
		case TLE5012B:
			sprintf(encoder_str, "TLE5012B");
			u8g2_DrawStr(&u8g2, 38, 46, encoder_str);
		break;
		
		case MT6816:
			sprintf(encoder_str, "MT6816");
			u8g2_DrawStr(&u8g2, 45, 46, encoder_str);
		break;
		
		default:
			sprintf(encoder_str, "Unknown");
			u8g2_DrawStr(&u8g2, 45, 46, encoder_str);	
		break;
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void Pole_Pairs(void)
{
	static uint8_t step = 0;
	char pp_str[10];
    
	if(step == 0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入极对数配置*/
		if(key1_event == KE_LongPress)
		{
			key1_event = KE_Dummy;
			step = 1;
		}
	}
	if(step == 1)
	{
		if(key1_event == KE_ShortPress)
		{
			MotorControl.motor_pole_pairs++;
			if(MotorControl.motor_pole_pairs > 30)
				MotorControl.motor_pole_pairs = 2;
		}
		if(key2_event == KE_ShortPress)
		{
			MotorControl.motor_pole_pairs--;
			if(MotorControl.motor_pole_pairs < 2)
				MotorControl.motor_pole_pairs = 30;
		}
		
		/*保存配置*/
		if(key1_event == KE_LongPress)
		{
			MotorControl.ModeNow = Save_Param;
			step = 0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step == 1)
	{
		u8g2_DrawTriangle(&u8g2, 96, 40, 91, 32, 91, 48);
		u8g2_DrawTriangle(&u8g2, 25, 40, 31, 32, 31, 48);
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tf);
	
	sprintf(pp_str, "%d", (int)MotorControl.motor_pole_pairs);
	u8g2_DrawStr(&u8g2, 52, 46, pp_str);
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void Default(void)
{
	static uint8_t step = 0;
	static uint8_t if_return_default;
	char default_str[10];
    
	if(step == 0)
	{
		u8g2_ClearBuffer(&u8g2);
		/*长按进入ID配置*/
		if(key1_event == KE_LongPress)
		{
			key1_event = KE_Dummy;
			step = 1;
		}
	}
	if(step == 1)
	{
		if(key1_event == KE_ShortPress)
		{
			if_return_default++;
			if(if_return_default > 1)
				if_return_default = 0;
		}
		if(key2_event == KE_ShortPress)
		{
			if_return_default--;
			if((int8_t)if_return_default < 0)
				if_return_default = 1;
		}
		
		/*保存配置*/
		if(key1_event == KE_LongPress && if_return_default == 1)
		{
			MotorControl.ModeNow = Default_Param;
			step = 0;
		}
	}

	/*只有在配置时有小箭头标识*/
	if(step == 1)
	{
		u8g2_DrawTriangle(&u8g2, 96, 40, 91, 32, 91, 48);
		u8g2_DrawTriangle(&u8g2, 25, 40, 31, 32, 31, 48);
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tf);
	
	if(if_return_default == 0)
	{
		sprintf(default_str, "No");
	}
	else if(if_return_default == 1)
	{
		sprintf(default_str, "Yes");
	}
	
	u8g2_DrawStr(&u8g2, 44, 46, default_str);
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
}

void Run(void)
{
	char Iq_ref_str[10],Iq_now_str[10];
	static uint8_t step;
	if(step == 0)
	{
		step = 1;
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_ncenB10_tf);
	u8g2_DrawStr(&u8g2, 10, 20, "Current");
	u8g2_DrawStr(&u8g2, 28, 36, "Mode");
	
	if(key1_event == KE_ShortPress)
	{
		MotorControl.speedRef += _2PI;
	}
	else if(key2_event == KE_ShortPress)
	{
		MotorControl.speedRef -= _2PI;
	}
	
	u8g2_SetFont(&u8g2, u8g2_font_6x10_mf);
	
	sprintf(Iq_ref_str, "Iq_ref  %3.2fA", MotorControl.iqRef);
	u8g2_DrawStr(&u8g2, 10, 50, Iq_ref_str);
	
	sprintf(Iq_now_str,"Iq_now  %3.2fA", MotorControl.iq);
	u8g2_DrawStr(&u8g2, 10, 62, Iq_now_str);
}

void Info(void)
{
	u8g2_DrawStr(&u8g2, 20, 35, "To be developed");
}
