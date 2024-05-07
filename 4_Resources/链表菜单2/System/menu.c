#include "stm32f10x.h"                  // Device header
#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include "Key.h"
#include "OLED.h"
#include "Draw.h"
#include "Delay.h"
#include "Draw_Font.h"

enum {NoKEY, ENTER, UP, DOWN, LEFT, RIGHT};

MENU* nowMenu;

typedef void (*Show_FunctionPtr)(int8_t, int8_t);
void Show_Function(Show_FunctionPtr Function, int8_t a, int8_t b)
{
    Function(a,b);
}

void Menu_Init(void)
{
    nowMenu = Creat_Menu("MENU",*Draw_Main_font,*NoFun);
        nowMenu = Creat_ChildMenu("Settings",*Draw_Settings_font,*Settings);
        nowMenu = Creat_BrotherMenu("Data",*Draw_Data_font,*NoFun);
			nowMenu = Creat_ChildMenu("Data_Lookup",*Draw_Data_Lookup_font,*Data_Lookup);
			nowMenu = Creat_BrotherMenu("Data_Change",*Draw_Data_Change_font,*Data_Change);
	        nowMenu = Creat_BrotherMenu("Data_Download",*Draw_Data_Download_font,*Data_Download);
			nowMenu = Creat_BrotherMenu("Data_Save",*Draw_Data_Save_font,*Data_Save);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu("Move",*Draw_Move_font,*NoFun);
			nowMenu = Creat_ChildMenu("Stop",*Draw_Stop_font,*Stop);
			nowMenu = Creat_BrotherMenu("Go_Super",*Draw_Go_Super_font,*Go_Super);
			nowMenu = Creat_BrotherMenu("Go_High",*Draw_Go_High_font,*Go_High);
			nowMenu = Creat_BrotherMenu("Go_Middle",*Draw_Go_Middle_font,*Go_Middle);
			nowMenu = Creat_BrotherMenu("Go_Slow",*Draw_Go_Slow_font,*Go_Slow);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu("Random",*Draw_Random_font,*Random);
        nowMenu = Circle_Menu();
    nowMenu = Circle_Menu();
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


void Choose_Menu(void)
{
    uint8_t key = Key_GetNum();
	MENU* now = nowMenu;
    switch(key)
    {
        case NoKEY:
            return;
        case ENTER:
            (*nowMenu->Function)();
            break;
        case UP:
            if(nowMenu->parent->parent == nowMenu->parent)
                break;
            nowMenu = nowMenu->parent;
            break;
        case DOWN:
            nowMenu = nowMenu->child;
            break;
        case LEFT:
            nowMenu = nowMenu->last;
            break;
        case RIGHT:
            nowMenu = nowMenu->next;
            break;
    }
	if(key!=NoKEY && nowMenu != now)
        Menu_Show(key);
}

void Menu_Show(uint8_t key)
{
	uint8_t i;
	MENU* now = nowMenu;
	
	for(i=0;i<8;i++)
    {
        if(key==LEFT)
        {
            OLED_Clear();
            Show_Function(nowMenu->Show, 1, -28+i*8);
            nowMenu = nowMenu->next;
            Show_Function(nowMenu->Show, 1, 40+i*8);
			nowMenu = nowMenu->next;
            Show_Function(nowMenu->Show, 1, 108+i*8);
			nowMenu = now->last;
            Show_Function(nowMenu->Show, 1, -96+i*8);
            Draw_Refresh_Set(0,1,127,6);
            Draw_Clear();
        }
        else if(key==RIGHT)
        {
            OLED_Clear();
            Show_Function(nowMenu->Show, 1, 108-i*8);
            nowMenu = nowMenu->last;
            Show_Function(nowMenu->Show, 1, 40-i*8);
			nowMenu = nowMenu->last;
            Show_Function(nowMenu->Show, 1, -28-i*8);
			nowMenu = now->next;
            Show_Function(nowMenu->Show, 1, 176-i*8);
            Draw_Refresh_Set(0,1,127,6);
            Draw_Clear();
        }
		else if(key==DOWN)
        {
            OLED_Clear();
            nowMenu = now;
            Show_Function(nowMenu->Show, -6+i, 40);
			nowMenu = now->last;
            Show_Function(nowMenu->Show, -6+i, 40-i*8);
			nowMenu = now->next;
            Show_Function(nowMenu->Show, -6+i, 40+i*8);
            Draw_Refresh_Set(0,0,127,6);
            Draw_Clear();
        }
		else if(key==UP)
        {
            OLED_Clear();
            nowMenu = now;
            Show_Function(nowMenu->Show, 8-i, 40);
			nowMenu = now->last;
            Show_Function(nowMenu->Show, 8-i, 40-i*8);
			nowMenu = now->next;
            Show_Function(nowMenu->Show, 8-i, 40+i*8);
            Draw_Refresh_Set(0,1,127,7);
            Draw_Clear();
        }
		
        Delay_ms(5);
        nowMenu = now;
    }

	OLED_Clear();
    Show_Function(nowMenu->Show, 1, 40);
    nowMenu = now->last;
    Show_Function(nowMenu->Show, 1, -28);
    nowMenu = now->next;
    Show_Function(nowMenu->Show, 1, 108);
    nowMenu = now;
	
	Draw_Triangle(40,31,32,23,32,39);
	Draw_Triangle(88,31,96,23,96,39);
	
    Draw_Refresh_Set(0,1,127,7);
	
	
	Show_Other();
	nowMenu = now;
}

void Show_Other(void)
{
	MENU* now = nowMenu;
	int8_t num=1;

	if(!strcmp(nowMenu->Name,"MENU"))
		OLED_ShowString_F6x8(8, (22-strlen("Made by Siberian_wolf"))/2*6, "Made By Siberian_wolf");
	else
		OLED_ShowString_F6x8(8, (22-strlen(nowMenu->Name))/2*6, nowMenu->Name);
	nowMenu = now->parent;
	OLED_ShowString_F6x8(1, (22-strlen(nowMenu->Name))/2*6, nowMenu->Name);
	MENU* firstMenu = nowMenu = nowMenu->child;
	
	if(strcmp(now->Name,"MENU"))
	{
		if(!nowMenu->flag)
			Draw_Slider(10,3,5,0);
		else if(nowMenu->flag)
			Draw_Slider(10,3,5,10);
		Draw_Refresh_Set(0,0,20,1);
		Draw_Clear();

		OLED_ShowChar_F6x8(1,109,'/');
		while(1)
		{
			if(nowMenu == firstMenu && num!=1)
			{
				OLED_ShowNum_F6x8(1,115,num-1,2);
				break;
			}
			if(nowMenu == now)
			{
				OLED_ShowNum_F6x8(1,97,num,2);
			}
			num++;
			nowMenu = nowMenu->next;
		}
	}
}

void NoFun(void)
{
	
}

void Settings(void)
{
	
}

void Data_Download(void)
{
	
}

void Data_Save(void)
{
	
}

void Stop(void)
{
	
}

void Go_Super(void)
{
	
}

void Go_High(void)
{
	
}

void Go_Middle(void)
{
	
}

void Go_Slow(void)
{
	
}

void Data_Lookup(void)
{
	
}

void Data_Change(void)
{
	
}

void Random(void)
{
	
}
