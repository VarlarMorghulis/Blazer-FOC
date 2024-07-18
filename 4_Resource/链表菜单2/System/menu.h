#ifndef __MENU_H
#define __MENU_H

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

void Menu_Init(void);

MENU* Creat_Menu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Creat_BrotherMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Creat_ChildMenu(char Name[], void (*Show)(int8_t, int8_t), void (*Function)());
MENU* Circle_Menu(void);

void Choose_Menu(void);
void Menu_Show(uint8_t key);
void Show_Other(void);

void Settings(void);
void Data_Download(void);
void Data_Save(void);
void Stop(void);
void Go_Super(void);
void Go_High(void);
void Go_Middle(void);
void Go_Slow(void);
void Data_Lookup(void);
void Data_Change(void);
void Random(void);
void NoFun(void);


#endif
