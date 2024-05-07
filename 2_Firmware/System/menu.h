#ifndef __MENU_H__
#define __MENU_H__

#include "common_inc.h"

typedef struct Menu
{
    char Name[20];

    void (*Show)(int8_t, int8_t);
    void (*Function)(void);

	uint8_t flag;
	
    struct Menu* last;
    struct Menu* next;
    struct Menu* parent;
    struct Menu* child;
}MENU;

void Menu_Display(void);
void Menu_Init(void);

MENU* Creat_Menu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Creat_BrotherMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Creat_ChildMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Circle_Menu(void);

uint8_t Menu_GetKeyval(void);
void Choose_Menu(void);
void Menu_Show(uint8_t key);

void Draw_Main_font(int8_t x, int8_t y);
void Draw_Calib_font(int8_t x, int8_t y);
void Draw_Setting_font(int8_t x, int8_t y);
void Draw_Run_font(int8_t x, int8_t y);
void Draw_Info_font(int8_t x, int8_t y);

void NoFun(void);
void Calib(void);
void Setting(void);
void Run(void);
void Info(void);

#endif