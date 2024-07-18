#ifndef __MENU_H
#define __MENU_H

typedef struct Menu
{
	uint8_t ID;
	int data;
	
	void (*Function)();
	
	struct Menu* last;
	struct Menu* next;
	struct Menu* parent;
	struct Menu* child;
}MENU;

void Menu_Init(void);

MENU* Creat_Menu(uint8_t ID, int data, void* Function);
MENU* Creat_BrotherMenu(uint8_t ID, int data, void* Function);
MENU* Creat_ChildMenu(uint8_t ID, int data, void* Function);
MENU* Circle_Menu(void);

void Choose_Menu(void);
void NoFun(void);


#endif
