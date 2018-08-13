#include "raylib.h"
#include "menu.h"
#include "sprites.h"
#include "sounds.h"
#include "game.h"
#include <stdio.h>

menu* menu_new(const char* title, int num_options, menu_option* options, int offset) {
	menu* m = malloc(sizeof(menu));
	m->title = title;
	m->options = options;
	m->num_options = num_options;
	m->offset = offset;
	m->selected = 0;

	return m;
}

void menu_free(menu* menu) {
	free(menu);
}

void menu_update(menu* menu, gamestate* state) {
	if (IsKeyPressed(KEY_UP)) {
		menu->selected--;
		play_sound(snd_menu1);
	} else if (IsKeyPressed(KEY_DOWN)) {
		menu->selected++;
		play_sound(snd_menu1);
	}

	if (menu->selected < 0) {
		menu->selected = menu->num_options - 1;
	} else if (menu->selected >= menu->num_options) {
		menu->selected = 0;
	}

	if (IsKeyPressed(KEY_ENTER)) {
		play_sound(snd_menu2);
		menu->options[menu->selected].callback(state);
	}
}

void menu_draw(menu* menu) {
	Vector2 screen_size = (Vector2) { GetScreenWidth(), GetScreenHeight() };
	int startY = screen_size.y / 4;

	// draw background bricks
	DrawTextureEx(img_bricks, (Vector2) { 130, 230 }, 0, cell_size / 16, (Color) { 255, 255, 255, 40 });
	DrawTextureEx(img_bricks2, (Vector2) { 600, 345 }, 0, cell_size / 16, (Color) { 255, 255, 255, 50 });

	Vector2 text_size = MeasureTextEx(font_main, menu->title, 60, 0);
	DrawTextEx(font_main, menu->title, (Vector2) { screen_size.x / 2 - text_size.x / 2 + 2, startY - text_size.y / 2 + 2 }, 60, 0, RED);
	DrawTextEx(font_main, menu->title, (Vector2) { screen_size.x / 2 - text_size.x / 2, startY - text_size.y / 2 }, 60, 0, RAYWHITE);
	startY += menu->offset;

	for (int i = 0; i < menu->num_options; i++) {
		char* txt = (menu->options[i]).text;
		int ypos = startY + menu->offset * (i + 1);

		Color clr = RAYWHITE;
		int fontsize = 30;
		if (i == menu->selected) {
			clr = BLUE;
			fontsize = 40;
		}

		
		Vector2 text_size = MeasureTextEx(font_main, txt, fontsize, 0);
		DrawTextEx(font_main, txt, (Vector2) { screen_size.x / 2 - text_size.x / 2, ypos - text_size.y / 2 }, fontsize, 0, clr);
	}

	const char* msg = "created in 48h for ludum dare 42";
	text_size = MeasureTextEx(font_main, msg, 18, 0);
	DrawTextEx(font_main, msg, (Vector2) { screen_size.x / 2 - text_size.x / 2, screen_size.y - text_size.y * 2 }, 18, 0, (Color) { 255, 255, 255, 180 });

}

