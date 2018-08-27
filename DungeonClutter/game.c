#include "game.h"
#include "main.h"
#include "sprites.h"
#include "sounds.h"
#include "list.h"
#include "utils.h"
#include <stdio.h>

int sound_enabled = 1;
game g;

void play_sound(Sound snd) {
	if (sound_enabled) {
		PlaySound(snd);
	}
}

void set_choices() {
	if (rand_int(0, 2) == 0) {
		//printf("Forced monster\n");
		g.choices[0] = object_types[rand_int(NUM_STATIC_TYPES, NUM_OBJECT_TYPES - 1)];
		g.choices[1] = object_types[rand_int(NUM_STATIC_TYPES, NUM_OBJECT_TYPES - 1)];
	} else {
		//printf("anything\n");
		g.choices[0] = object_types[rand_int(NUM_PROTECTED_TYPES, NUM_OBJECT_TYPES - 1)];
		g.choices[1] = object_types[rand_int(NUM_PROTECTED_TYPES, NUM_OBJECT_TYPES - 1)];
	}
}

int start_level_from_file(char* filename) {
	char c;
	int num = 0;
	int cur_num = 0;
	char* filepath = get_file_path(filename);
	FILE* fp;
	printf("opening: %s\n", filepath);
	int err = fopen_s(&fp, filepath, "r");
	printf("err: %d\n", err);
	free(filepath);

	int lvl_width = 0, lvl_height = 0;
	int plr_x = 0, plr_y;

	if (err == 0) {
		while ((c = getc(fp)) != EOF) {
			if (cur_num < 4) {
				if (c >= '0' && c <= '9') {
					num = num * 10 + (c - '0');
				} else {
					if (cur_num == 0) {
						lvl_width = num;
					} else if (cur_num == 1) {
						lvl_height = num;
					} else if (cur_num == 2) {
						plr_x = num;
					} else if (cur_num == 3) {
						plr_y = num;
					}

					cur_num++;
					num = 0;
				}
			} else {
				break;
			}
		}

		g.plr_position = (Vector2) { plr_x, plr_y };
		g.plr_health = g.plr_maxhealth;
		g.curmove_type = MOVE_PLR;
		g.chosen_type = -1;
		g.selected_cell = (Vector2) { -1, -1 };

		level* lvl = malloc(sizeof(level));
		lvl->objects = list_new();

		lvl->width = lvl_width;
		lvl->height = lvl_height;
		lvl->data = malloc(lvl->width * sizeof(int*));

		for (int x = 0; x < lvl->width; x++) {
			lvl->data[x] = malloc(lvl->height * sizeof(int));
			for (int y = 0; y < lvl->height; y++) {
				if (c != EOF) {
					int cell_type = c - '0';
					if (cell_type == CELL_KEY) {
						lvl->data[x][y] = CELL_OBJ;
						list_push(lvl->objects, *object_new(object_types[0], (Vector2) { x, y }, object_types[0].health));
					} else if (cell_type == CELL_DOOR) {
						lvl->data[x][y] = CELL_OBJ;
						list_push(lvl->objects, *object_new(object_types[1], (Vector2) { x, y }, object_types[1].health));
					} else {
						lvl->data[x][y] = c - '0';
					}

					c = getc(fp);
				} else {
					printf("error, not enough chars for this map\n");
					exit(1);
				}
			}
		}

		fclose(fp);
		set_choices();
		g.lvl = lvl;

		return 0;
	} else {
		printf("level not found! (%s)\n", filepath);
		exit(2);
	}
}

void start_game(gamestate* state) {
	g.lvl_num = 1;
	g.plr_maxhealth = 5;

	char* filepath = malloc(100 * sizeof(char));
	snprintf(filepath, 100, "levels\\level%d.lvl", g.lvl_num);
	g.file_path = filepath;

	start_level_from_file(g.file_path);
	*state = GAME;
}

void view_options(gamestate* state) {
	*state = OPTIONS;
}

void view_credits(gamestate* state) {
	*state = CREDITS;
}

void back_to_menu(gamestate* state) {
	*state = MENU;
}

void quit_game(gamestate* state) {
	exit(0);
}

void toggle_sound(gamestate* state) {
	sound_enabled = sound_enabled ? 0 : 1;
}

int empty_cell(int celltype) {
	return celltype == CELL_EMPTY;
}

int empty_or_obj(int x, int y) {
	return valid(x, y) && ((empty_cell(g.lvl->data[x][y])) || g.lvl->data[x][y] == CELL_OBJ);
}

int can_place(int x, int y) {
	return valid(x, y) && empty_cell(g.lvl->data[x][y]) && !(x == g.plr_position.x && y == g.plr_position.y);
}

int valid(int x, int y) {
	return x >= 0 && x < g.lvl->width && y >= 0 && y < g.lvl->height;
}

int valid_move(int x, int y) {
	return valid(x, y) && empty_cell(g.lvl->data[x][y]);
}

void next_level() {
	play_sound(snd_menu2);

	// do animation of top and bottom closing in
	animating_transition = 1;
	animating_to_level = g.lvl_num++;
}

object* object_at(int x, int y) {
	for (int i = 0; i < g.lvl->objects->length; i++) {
		object* o = list_get(g.lvl->objects, i);
		if (o->position.x == x && o->position.y == y) {
			return o;
		}
	}
	return NULL;
}

int flood_fill(list* l, int** visited, int x, int y) {
	list_push(l, *object_new(object_types[0], (Vector2) { x, y }, 0));
	visited[x][y] = 1;
	for (int i = 0; i < 4; i++) {
		Vector2 new_pos = (Vector2) { x + dirs[i].x, y + dirs[i].y };
		if (valid(new_pos.x, new_pos.y) && !visited[(int)new_pos.x][(int)new_pos.y]) {
			object* o = object_at(new_pos.x, new_pos.y);

			if (can_place(new_pos.x, new_pos.y)) {
				flood_fill(l, visited, new_pos.x, new_pos.y);
			} else {
				if (o == NULL) {
					list_push(l, *object_new(object_types[0], (Vector2) { new_pos.x, new_pos.y }, 0));
				} else if (o->max_health <= 0) {
					list_push(l, *object_new(object_types[0], (Vector2) { new_pos.x, new_pos.y }, 0));
				} else {
					flood_fill(l, visited, new_pos.x, new_pos.y);
				}
			}
		}
	}
}

list* start_flood_fill(int x, int y) {
	list* flood = list_new();
	int** visited = malloc(g.lvl->width * sizeof(int*));
	for (int i = 0; i < g.lvl->width; i++) {
		visited[i] = calloc(g.lvl->height, sizeof(int));
	}

	// flood fill
	flood_fill(flood, visited, x, y);

	// free visited array
	for (int i = 0; i < g.lvl->width; i++) {
		free(visited[i]);
	}
	free(visited);

	return flood;
}

void move_plr(int dir) {
	Vector2 vec = dirs[dir];
	if (valid(g.plr_position.x + vec.x, g.plr_position.y + vec.y)) {
		Vector2 cell_pos = (Vector2) { g.plr_position.x + vec.x, g.plr_position.y + vec.y };
		cell_type cell = g.lvl->data[(int)cell_pos.x][(int)cell_pos.y];
		interact_result result = INTERACT_CONTINUE;

		if (valid_move(cell_pos.x, cell_pos.y) || cell == CELL_OBJ) {
			if (valid_move(cell_pos.x, cell_pos.y)) {
				g.plr_position = (Vector2) { cell_pos.x, cell_pos.y };
				play_sound(snd_move1);
			} else if (cell == CELL_OBJ) {
				// interact
				for (int i = 0; i < g.lvl->objects->length; i++) {
					object* obj = list_get(g.lvl->objects, i);
					if (obj->position.x == cell_pos.x && obj->position.y == cell_pos.y) {
						result = obj->type.interact(obj, &g);
						//printf("result is %d\n", result);
						if (result != INTERACT_NONE) {
							if (result == INTERACT_DELETE) {
								play_sound(snd_menu2);
								list_remove(g.lvl->objects, i);
								g.lvl->data[(int)cell_pos.x][(int)cell_pos.y] = CELL_EMPTY;
							} else {
								play_sound(snd_move1);
							}
							g.curmove_type = MOVE_CHOOSE;
							break;
						}
					}
				}
			}

			for (int i = 0; i < g.lvl->objects->length; i++) {
				object* o = list_get(g.lvl->objects, i);
				o->type.step(o, &g);

				if (g.plr_health <= 0) { // player died
					play_sound(snd_explode);
					start_level_from_file(g.file_path);

					return;
				}
			}
			//printf("result is now %d\n", result);
			//printf("result is %d, interact_none is %d\n", result, INTERACT_NONE);
			if (result != INTERACT_NONE) {
				//printf("they are not equal, moving on...\n");
				g.curmove_type = MOVE_CHOOSE;
			}
		} else if (cell == CELL_WIN) {
			next_level();
		}
	}

	
}

int mouse_within(Vector2 mouse_pos, Vector2 position, Vector2 size) {
	return mouse_pos.x >= position.x && mouse_pos.x <= position.x + size.x &&
		mouse_pos.y >= position.y && mouse_pos.y <= position.y + size.y;
}

void check_disconnected_cells() {
	// flood fill on player, delete missing squares
	int removedPiece = 0;

	list* flood = start_flood_fill(g.plr_position.x, g.plr_position.y);
	for (int x = 0; x < g.lvl->width; x++) {
		for (int y = 0; y < g.lvl->height; y++) {
			int in_list = 0;
			for (int i = 0; i < flood->length; i++) {
				object* o = list_get(flood, i);
				if ((int)o->position.x == x && (int)o->position.y == y) {
					in_list = 1;
					break;
				}
			}

			if (!in_list && g.lvl->data[x][y] != CELL_NULL) {
				for (int i = 0; i < g.lvl->objects->length; i++) {
					object* o = list_get(g.lvl->objects, i);
					if ((int)o->position.x == x && (int)o->position.y == y) {
						list_remove(g.lvl->objects, i);
						break;
					}
				}
				removedPiece = 1;
				g.lvl->data[x][y] = CELL_NULL;

			}
		}
	}

	if (removedPiece) {
		play_sound(snd_explode);
	}
}

void game_update() {
	if (animating_transition) {
		if (anim_alpha >= 1 && animating_to_level) {
			animating_to_level = 0;

			// free previous level
			for (int i = 0; i < g.lvl->width; i++) {
			free(g.lvl->data[i]);
			}
			free(g.lvl->data);
			list_free(g.lvl->objects);
			free(g.lvl);
			free(g.file_path);

			// load the next level
			char* filepath = malloc(100 * sizeof(char));
			snprintf(filepath, 100, "levels\\level%d.lvl", g.lvl_num);
			//printf("getting file: %s\n", filepath);

			start_level_from_file(filepath);
			g.file_path = filepath;

		}
		// dont do anything
	} else if (IsKeyPressed(KEY_R)) {
		play_sound(snd_menu2);
		start_level_from_file(g.file_path);
	} else {

		Vector2 choice_size = (Vector2) { 100, 100 };
		Vector2 screen_size = (Vector2) { GetScreenWidth(), GetScreenHeight() };
		Vector2 mouse_pos = GetMousePosition();
		Vector2 choice_pos[] = {
			{ screen_size.x / 2 - choice_size.x * 1.5, screen_size.y - choice_size.y },
			{ screen_size.x / 2 + choice_size.x / 2, screen_size.y - choice_size.y },
		};

		Vector2 level_origin = (Vector2) { screen_size.x / 2 - g.lvl->width * (cell_size + cell_padding) / 2, screen_size.y / 2 - g.lvl->height * (cell_size + cell_padding) / 2 };
		Vector2 mouse_dist_from_level_origin = (Vector2) { mouse_pos.x - level_origin.x, mouse_pos.y - level_origin.y };
		Vector2 selected_cell = (Vector2) { (int)(mouse_dist_from_level_origin.x / (cell_size + cell_padding)), (int)(mouse_dist_from_level_origin.y / (cell_size + cell_padding)) };

		switch (g.curmove_type) {
			case MOVE_PLR: {
				if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
					move_plr(RIGHT);
				} else if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
					move_plr(LEFT);
				} else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
					move_plr(UP);
				} else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
					move_plr(DOWN);
				}
				break;
			}
			case MOVE_CHOOSE: {
				// if player clicks
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					if (mouse_within(mouse_pos, choice_pos[0], choice_size)) {
						// if mouse is within choice1 rectangle
						g.chosen_type = 0;
						g.curmove_type = MOVE_PLACE;
					} else if (mouse_within(mouse_pos, choice_pos[1], choice_size)) {
						// elseif mouse is within choice2 rectangle
						g.chosen_type = 1;
						g.curmove_type = MOVE_PLACE;
					}
				}
				break;
			}
			case MOVE_PLACE: {
				// if player moused over a cell, set that cell as selected
				if (valid(selected_cell.x, selected_cell.y)) {
					g.selected_cell = (Vector2) { selected_cell.x, selected_cell.y };
					if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && can_place(selected_cell.x, selected_cell.y)) {
						object* obj = object_new(g.choices[g.chosen_type], selected_cell, g.choices[g.chosen_type].health);
						list_push(g.lvl->objects, *obj);
						g.lvl->data[(int)selected_cell.x][(int)selected_cell.y] = CELL_OBJ;
						object_free(obj);
						g.chosen_type = -1;
						g.selected_cell = (Vector2) { -1, -1 };
						set_choices();

						check_disconnected_cells();

						g.curmove_type = MOVE_PLR;
					}
				}

				break;
			}
		}
	}
}

void game_draw(int tick) {
	Vector2 mouse_pos = GetMousePosition();
	Vector2 screen_size = (Vector2) { GetScreenWidth(), GetScreenHeight() };
	Vector2 map_origin = (Vector2) { screen_size.x / 2 - g.lvl->width * (cell_size + cell_padding) / 2, screen_size.y / 2 - g.lvl->height * (cell_size + cell_padding) / 2 };
	int square_size = cell_size / SPRITE_SIZE;

	// draw background bricks
	DrawTextureEx(img_bricks, (Vector2) { 100, 86 }, 0, square_size, (Color) { 255, 255, 255, 45 });
	DrawTextureEx(img_bricks, (Vector2) { 80, 380 }, 0, square_size, (Color) { 255, 255, 255, 50 });
	DrawTextureEx(img_bricks, (Vector2) { 730, 20 }, 0, square_size, (Color) { 255, 255, 255, 40 });

	DrawTextureEx(img_bricks2, (Vector2) { 40, 245 }, 0, square_size, (Color) { 255, 255, 255, 50 });
	DrawTextureEx(img_bricks2, (Vector2) { 300, 45 }, 0, square_size, (Color) { 255, 255, 255, 40 });
	DrawTextureEx(img_bricks2, (Vector2) { 680, 245 }, 0, square_size, (Color) { 255, 255, 255, 55 });

	// draw level cells
	for (int x = 0; x < g.lvl->width; x++) {
		for (int y = 0; y < g.lvl->height; y++) {
			int cell = g.lvl->data[x][y];
			if (cell != CELL_NULL) {
				Texture2D* cell_tex = &img_cell;
				if ((x * y) % 5 == 1) {
					cell_tex = &img_cell2;
				}

				Color tint = WHITE;
				if (x == g.selected_cell.x && y == g.selected_cell.y) {
					if (can_place(x, y)) {
						tint = GREEN;
					} else {
						tint = RED;
					}
				}

				DrawTextureEx(*cell_tex, (Vector2) { map_origin.x + x * (cell_size + cell_padding), map_origin.y + y * (cell_size + cell_padding) }, 0, square_size, tint);
				if (cell == CELL_WIN) {
					DrawTextureEx(img_stairs, (Vector2) { map_origin.x + x * (cell_size + cell_padding), map_origin.y + y * (cell_size + cell_padding) }, 0, square_size, WHITE);
				}
			}
		}
	}

	// draw clutter
	for (int i = 0; i < g.lvl->objects->length; i++) {
		object* obj = list_get(g.lvl->objects, i);

		Texture2D* obj_tex = &obj->type.anim.textures[tick % (obj->type.anim.animation_time * obj->type.anim.num_textures) / obj->type.anim.animation_time];
		Vector2 obj_pos = (Vector2) { map_origin.x + obj->position.x * (cell_size + cell_padding), map_origin.y + obj->position.y * (cell_size + cell_padding) };
		DrawTextureEx(*obj_tex, obj_pos, 0, square_size, WHITE);

		if (obj->damaged) {
			DrawTextEx(font_main, "-1", (Vector2) {obj_pos.x + cell_size / 4, obj_pos.y - cell_size / 4 - (40 - obj->damaged) / 2}, 24, 0, (Color) { 200, 0, 0, 255 });
			obj->damaged--;
		}
	}

	// draw player moves
	if (g.curmove_type == MOVE_PLR) {
		for (int i = 0; i < 4; i++) {
			Vector2 vec = dirs[i];
			Vector2 cell_pos = (Vector2) { g.plr_position.x + vec.x, g.plr_position.y + vec.y };
			if (valid_move(g.plr_position.x + vec.x, g.plr_position.y + vec.y)) {
				DrawTextureEx(img_canmove1, (Vector2) { map_origin.x + cell_pos.x * (cell_size + cell_padding), map_origin.y + cell_pos.y * (cell_size + cell_padding) }, 0, square_size, WHITE);
			}
		}
	}


	Texture2D* plr_tex = &img_player;
	if (tick % 40 >= 20) {
		plr_tex = &img_player2; // primitive two-state animation
	}
	// draw player
	DrawTextureEx(*plr_tex, (Vector2) { map_origin.x + g.plr_position.x * (cell_size + cell_padding), map_origin.y + g.plr_position.y * (cell_size + cell_padding) }, 0, square_size, WHITE);
	
	// bottom bar
	DrawRectangleV((Vector2) { 0, screen_size.y - 120 }, (Vector2) { screen_size.x, 120 }, BGCOLOR_DARK);

	// clutter options
	Vector2 choice_size = (Vector2) {100, 100};

	DrawTextureEx(g.choices[0].anim.textures[0], (Vector2) { screen_size.x / 2 - choice_size.x * 1.5, screen_size.y - choice_size.y }, 0, choice_size.x / SPRITE_SIZE, WHITE);
	DrawTextureEx(g.choices[1].anim.textures[0], (Vector2) { screen_size.x / 2 + choice_size.x / 2, screen_size.y - choice_size.y }, 0, choice_size.x / SPRITE_SIZE, WHITE);

	DrawTextEx(font_main, g.choices[0].name, (Vector2) { screen_size.x / 2 - choice_size.x * 1.5, screen_size.y - choice_size.y }, 20, 0, WHITE);
	DrawTextEx(font_main, g.choices[1].name, (Vector2) { screen_size.x / 2 + choice_size.x / 2, screen_size.y - choice_size.y }, 20, 0, WHITE);


	// heath indicator
	
	int health_width = (cell_size + cell_padding) * g.plr_maxhealth;
	int plr_hearts = g.plr_health;
	for (int i = 0; i < g.plr_maxhealth; i++) {
		Texture2D* heart_tex = &img_heart_empty;
		if (plr_hearts) {
			plr_hearts--;
			heart_tex = &img_heart;
		}

		DrawTextureEx(*heart_tex, (Vector2) {screen_size.x / 2 - health_width / 2 + i * (cell_size + cell_padding), 0}, 0, square_size, WHITE);
	}

	// animated transition
	if (animating_transition) {
		// TODO: make this images
		float real_alpha = anim_alpha;
		if (anim_alpha < 1) {
			real_alpha = anim_alpha;
		} else {
			real_alpha = 1 - (anim_alpha - 1);
		} 

		DrawRectangleV((Vector2) { 0, 0 }, (Vector2) { screen_size.x, real_alpha * (screen_size.y / 2) }, TRANSITION_COLOR);
		DrawRectangleV((Vector2) { 0, screen_size.y - (real_alpha * (screen_size.y / 2)) }, (Vector2) { screen_size.x, real_alpha * (screen_size.y / 2) }, TRANSITION_COLOR);

		anim_alpha += 0.025;
		if (anim_alpha >= 2) {
			animating_transition = 0;
			anim_alpha = 0.0f;
		}
	}



	// draw cursor
	DrawTextureEx(img_cursor, mouse_pos, 0, 2, WHITE);
}