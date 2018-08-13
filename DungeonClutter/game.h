#pragma once

#include "main.h"
#include "menu.h"
#include "list.h"
#include "raylib.h"

typedef enum move_type {
	MOVE_PLR, MOVE_CHOOSE, MOVE_PLACE
} move_type;

typedef enum cell_type {
	CELL_NULL, CELL_EMPTY, CELL_WIN, CELL_OBJ, CELL_DOOR, CELL_KEY
} cell_type;

typedef enum directions {
	RIGHT, LEFT, DOWN, UP
} directions;

typedef struct level {
	int width;
	int height;
	int** data;
	list* objects;
} level;

typedef struct game {
	int lvl_num;
	level* lvl;
	Vector2 plr_position;
	int plr_health;
	int plr_maxhealth;
	object_type choices[2];
	move_type curmove_type;
	int chosen_type;
	Vector2 selected_cell;
	char* file_path;
} game;

void start_game(gamestate* state);
void view_options(gamestate* state);
void toggle_sound(gamestate* state);
void view_credits(gamestate* state);
void back_to_menu(gamestate* state);
void quit_game(gamestate* state);

void play_sound(Sound snd);
void game_update();
void game_draw(int tick);
int can_place(int x, int y);
int valid(int x, int y);
int valid_move(int x, int y);
void check_disconnected_cells();
object* object_at(int x, int y);
int empty_or_obj(int x, int y);

void next_level();

extern game g;
static int cell_size = 55;
static int cell_padding = 3;
extern int sound_enabled;
int animating_transition;
float anim_alpha;
int animating_to_level;

static menu_option main_menu_options[] = {
	{ "Play", &start_game },
	{ "Options", &view_options },
	{ "Credits", &view_credits },
	{ "Quit", &quit_game },
};

static menu_option options_menu_options[] = {
	{ "Sound - On", &toggle_sound },
	{ "Back", &back_to_menu },
};

static menu_option credits_options[] = {
	{ "Back", &back_to_menu },
};

static Vector2 dirs[4] = {
	{ 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 }
};

// interact effects
interact_result no_effect(object* obj, game* g);
interact_result attack(object* obj, game* g);
interact_result heal_player(object* obj, game* g);
interact_result open_door(object* obj, game* g);

// step effects
void move_to_player(object* obj, game* g);