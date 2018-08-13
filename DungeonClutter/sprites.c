#include "sprites.h"
#include "utils.h"
#include <stdio.h>

Texture2D texture_load(const char* filename) {
	char* fp = get_file_path(filename);
	Texture2D tex = LoadTexture(fp);
	free(fp);

	return tex;
}

Font font_load(const char* filename) {
	char* fp = get_file_path(filename);
	Font font = LoadFont(fp);
	free(fp);

	return font;
}

void sprites_init() {
	font_main = font_load("resources\\superstar_memesbruh03.ttf");

	img_cursor = texture_load("resources\\cursor.png");
	img_heart = texture_load("resources\\heart.png");
	img_heart_empty = texture_load("resources\\heart_empty.png");
	img_cell = texture_load("resources\\cell.png");
	img_cell2 = texture_load("resources\\cell2.png");
	img_player = texture_load("resources\\player.png");
	img_player2 = texture_load("resources\\player2.png");
	img_canmove1 = texture_load("resources\\canmove1.png");

	img_chest = texture_load("resources\\chest.png");
	img_table = texture_load("resources\\table.png");
	img_bookshelf = texture_load("resources\\bookshelf.png");
	img_slime = texture_load("resources\\slime.png");
	img_slime2 = texture_load("resources\\slime2.png");
	img_knight = texture_load("resources\\knight.png");
	img_knight2 = texture_load("resources\\knight2.png");
	img_potion = texture_load("resources\\potion.png");
	img_key = texture_load("resources\\key.png");
	img_key2 = texture_load("resources\\key2.png");
	img_door = texture_load("resources\\door.png");

	img_stairs = texture_load("resources\\stairs.png");
	img_bricks = texture_load("resources\\bricks.png");
	img_bricks2 = texture_load("resources\\bricks2.png");
}

void sprites_unload() {
	UnloadFont(font_main);

	UnloadTexture(img_cursor);
	UnloadTexture(img_heart);
	UnloadTexture(img_cell);
	UnloadTexture(img_cell2);
	UnloadTexture(img_player);
	UnloadTexture(img_player2);
	UnloadTexture(img_canmove1);

	UnloadTexture(img_chest);
	UnloadTexture(img_table);
	UnloadTexture(img_bookshelf);
	UnloadTexture(img_slime);
	UnloadTexture(img_slime2);
	UnloadTexture(img_knight);
	UnloadTexture(img_knight2);

	UnloadTexture(img_stairs);
	UnloadTexture(img_bricks);
	UnloadTexture(img_bricks2);
}