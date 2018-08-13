#pragma once

#include "main.h"

typedef struct menu_option {
	const char* text;
	void(*callback)(gamestate*);
} menu_option;

typedef struct menu {
	const char* title;
	menu_option* options;
	int num_options;
	int offset;

	int selected;
} menu;

menu* menu_new(const char* title, int num_options, menu_option* options, int offset);
void menu_update(menu* menu, gamestate* state);
void menu_draw(menu* menu);