#include <stdio.h>
#include "raylib.h"
#include "sprites.h"
#include "sounds.h"
#include "objects.h"
#include "menu.h"
#include "game.h"
#include "main.h"
#include "list.h"

int main(int argc, char** argv) {
	exe_dir = argv[0];

	// initialize window
	InitWindow(800, 600, TITLE);
	InitAudioDevice();
	SetTargetFPS(FPS);

	// initialize game
	int tick = 0;
	sprites_init();
	sounds_init();
	objects_init();
	HideCursor();

	menu* main_menu = menu_new("Clutter Dungeon", 4, main_menu_options, 60);
	menu* options_menu = menu_new("Options", 2, options_menu_options, 60);
	menu* credits_menu = menu_new("  Made by Kodran\nFor ludum dare 42", 1, credits_options, 120);
	gamestate state = MENU;

	while (!WindowShouldClose()) {
		// update

		switch (state) {
		case MENU:
			menu_update(main_menu, &state);
			break;
		case OPTIONS:
			menu_update(options_menu, &state);
			options_menu->options[0].text = sound_enabled ? "Sound - On" : "Sound - Off";
			break;
		case CREDITS:
			menu_update(credits_menu, &state);
			break;
		case GAME:
			game_update();
			break;
		}

		// draw
		BeginDrawing();

		ClearBackground(BGCOLOR);
		switch (state) {
		case MENU:
			menu_draw(main_menu);
			Vector2 text_size = MeasureTextEx(font_main, main_menu->title, 60, 0);
			DrawTextureEx(img_player, (Vector2) { WIDTH / 2 - text_size.x / 2 - (16 * 6), HEIGHT / 4 - text_size.y / 2 - (16) }, 0, 6, WHITE);
			break;
		case OPTIONS:
			menu_draw(options_menu);
			break;
		case CREDITS:
			menu_draw(credits_menu);
			break;
		case GAME:
			game_draw(tick);
			break;
		}

		DrawFPS(0, 0);
		EndDrawing();

		tick++;
	}

	menu_free(main_menu);
	menu_free(options_menu);
	menu_free(credits_menu);

	sprites_unload();
	sounds_unload();

	CloseAudioDevice();
	CloseWindow();

	return 0;
}