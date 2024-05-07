#include "stm32f10x.h"                  // Device header
#include "menu.h"
#include <stdlib.h>
#include <string.h>
#include "Key.h"
#include "OLED.h"

enum {NoKEY, ENTER, UP, DOWN, LEFT, RIGHT};

char show[][20] = {
	"MENU","Settings","Data","Download","Save",
	"childMenu01","childMenu02","childMenu03",
	"childMenu011","childMenu012","childMenu013"
};

MENU* nowMenu;

void Menu_Init(void)
{
	nowMenu = Creat_Menu(0,0,*NoFun);
		nowMenu = Creat_ChildMenu(1,0,*NoFun);
			nowMenu = Creat_ChildMenu(5,0,*NoFun);
				nowMenu = Creat_ChildMenu(8,0,*NoFun);
				nowMenu = Creat_BrotherMenu(9,0,*NoFun);
				nowMenu = Creat_BrotherMenu(10,0,*NoFun);
				nowMenu = Circle_Menu();
			nowMenu = Creat_BrotherMenu(6,0,*NoFun);
			nowMenu = Creat_BrotherMenu(7,0,*NoFun);
			nowMenu = Circle_Menu();
		nowMenu = Creat_BrotherMenu(2,0,*NoFun);
		nowMenu = Creat_BrotherMenu(3,0,*NoFun);
		nowMenu = Creat_BrotherMenu(4,0,*NoFun);
		nowMenu = Circle_Menu();
	nowMenu = Circle_Menu();
}

MENU* Creat_Menu(uint8_t ID, int data, void* Function)
{
    MENU* node = malloc(sizeof(MENU));
	node->ID = ID;
    node->data = data;
	node->Function = Function;
	
    node->last = NULL;
    node->next = NULL;
	node->child = node;
	node->parent = node;
	
	//nowMenu=node;
    return node;
}

MENU* Creat_ChildMenu(uint8_t ID, int data, void* Function)
{
    MENU* node = malloc(sizeof(MENU));
	node->ID = ID;
    node->data = data;
	node->Function = Function;
	
    node->last = NULL;
    node->next = NULL;
	node->child = node;
	node->parent = nowMenu;
	
	nowMenu->child = node;
	
    return node;
}

MENU* Creat_BrotherMenu(uint8_t ID, int data, void* Function)
{
    MENU* node = malloc(sizeof(MENU));
	node->ID = ID;
    node->data = data;
	node->Function = Function;
	
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
	switch(key)
	{
		case NoKEY:
			return;
		case ENTER:
			(*nowMenu->Function)();
			break;
		case UP:
			nowMenu = nowMenu->last;
			break;
		case DOWN:
			nowMenu = nowMenu->next;
			break;
		case LEFT:
			nowMenu = nowMenu->parent;
			break;
		case RIGHT:
			nowMenu = nowMenu->child;
			break;
	}
	MENU* now = nowMenu;
	
	OLED_Clear();
	OLED_ShowString2(3,1,show[nowMenu->ID]);
	nowMenu = now->next;
	OLED_ShowString(4,1,show[nowMenu->ID]);
	nowMenu = now->last;
	OLED_ShowString(2,1,show[nowMenu->ID]);
	nowMenu = now->parent;
	OLED_ShowString(1,(16-strlen(show[nowMenu->ID]))/2+1,show[nowMenu->ID]);
	
	nowMenu = now;
}


void NoFun(void)
{
	
}