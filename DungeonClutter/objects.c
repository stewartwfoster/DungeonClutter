#include "objects.h"
#include "sprites.h"
#include "game.h"
#include "list.h"
#include "utils.h"
#include <string.h>

object_type object_types[NUM_OBJECT_TYPES];

void objects_init() {

	// PROTECTED OBJECT TYPES

	Texture2D* key_textures = malloc(sizeof(Texture2D) * 2);
	key_textures[0] = img_key;
	key_textures[1] = img_key2;
	object_types[0] = (object_type) { "Key", { key_textures, 2, 20 }, 2, &open_door, &no_effect };

	Texture2D* door_textures = malloc(sizeof(Texture2D) * 1);
	door_textures[0] = img_door;
	object_types[1] = (object_type) { "Door", { door_textures, 1, 1 }, 2, &no_effect, &no_effect };

	// STATIC OBJECT TYPES

	Texture2D* chest_textures = malloc(sizeof(Texture2D) * 1);
	chest_textures[0] = img_chest;
	object_types[2] = (object_type) { "Chest", { chest_textures, 1, 1 }, -1, &no_effect, &no_effect };

	Texture2D* table_textures = malloc(sizeof(Texture2D) * 1);
	table_textures[0] = img_table;
	object_types[3] = (object_type) { "Table", { table_textures, 1, 1 }, -1, &no_effect, &no_effect };

	Texture2D* bookshelf_textures = malloc(sizeof(Texture2D) * 1);
	bookshelf_textures[0] = img_bookshelf;
	object_types[4] = (object_type) { "Bookshelf", { bookshelf_textures, 1, 1 }, -1, &no_effect, &no_effect };

	Texture2D* potion_textures = malloc(sizeof(Texture2D) * 1);
	potion_textures[0] = img_potion;
	object_types[5] = (object_type) { "Potion", { potion_textures, 1, 1 }, 2, &heal_player, &no_effect };

	// MONSTER OBJECT TYPES

	Texture2D* slime_textures = malloc(sizeof(Texture2D) * 2);
	slime_textures[0] = img_slime;
	slime_textures[1] = img_slime2;
	object_types[6] = (object_type) { "Slime", { slime_textures, 2, 20 }, 2, &attack, &move_to_player };

	Texture2D* knight_textures = malloc(sizeof(Texture2D) * 2);
	knight_textures[0] = img_knight;
	knight_textures[1] = img_knight2;
	object_types[7] = (object_type) { "Knight", { knight_textures, 2, 20 }, 4, &attack, &move_to_player };
}

object* object_new(object_type type, Vector2 position, int health) {
	object* obj = malloc(sizeof(object));
	obj->type = type;
	obj->position = position;
	obj->health = health;
	obj->max_health = health;
	obj->damaged = 0;

	return obj;
}

void object_free(object* obj) {
	free(obj);
}

interact_result no_effect(object* obj, game* g) {
	return INTERACT_NONE;
}

interact_result open_door(object* obj, game* g) {
	// go through g.lvl->objects and delete any doors
	for (int i = 0; i < g->lvl->objects->length; i++) {
		object* o = list_get(g->lvl->objects, i);
		if (strcmp(o->type.name, "Door") == 0) {
			g->lvl->data[(int)o->position.x][(int)o->position.y] = CELL_EMPTY;
			list_remove(g->lvl->objects, i);
			i = 0;
		}
	}
	return INTERACT_DELETE;
}

interact_result heal_player(object* obj, game* g) {
	if (g->plr_health + 2 > g->plr_maxhealth) {
		g->plr_health += 2;
	} else {
		g->plr_health = g->plr_maxhealth;
	}
	return INTERACT_DELETE;
}

interact_result attack(object* obj, game* g) {
	// todo: attack hurts player
	obj->health--;
	if (obj->health <= 0) { // killed object
		return INTERACT_DELETE;
	}

	obj->damaged = 40;
	return INTERACT_CONTINUE;
}


void flood_fill_primer(int** visited, int x, int y, int move) {
	for (int i = 0; i < 4; i++) {
		Vector2 new_pos = (Vector2) { x + dirs[i].x, y + dirs[i].y };
		object* o = object_at(new_pos.x, new_pos.y);
		if (empty_or_obj(new_pos.x, new_pos.y) && !visited[(int)new_pos.x][(int)new_pos.y] && (o == (void*)0 || o->max_health > 0)) {
			visited[(int)new_pos.x][(int)new_pos.y] = move + 1;
		}
	}

	for (int i = 0; i < 4; i++) {
		Vector2 new_pos = (Vector2) { x + dirs[i].x, y + dirs[i].y };

		if (valid(new_pos.x, new_pos.y)) {
			if (visited[(int)new_pos.x][(int)new_pos.y] == move + 1) {
				flood_fill_primer(visited, new_pos.x, new_pos.y, move + 1);
			}
		}
	}
}

list* flood_fill_search(list* l, int** visited, Vector2 start, Vector2 goal) {
	// start is plrpos, goal is objpos
	//printf("starting at %f, %f\n", start.x, start.y);

	Vector2 cur_pos = (Vector2) { start.x, start.y };
	while (!(cur_pos.x == goal.x && cur_pos.y == goal.y)) {
		int min_val = 99999999;
		int min_dir = 0;
		for (int i = 0; i < 4; i++) {
			Vector2 new_pos = (Vector2) { cur_pos.x + dirs[i].x, cur_pos.y + dirs[i].y };
			if (valid(new_pos.x, new_pos.y)) {
				int vis = visited[(int)new_pos.x][(int)new_pos.y];
				if (vis > 0 && vis < min_val) {
					min_val = visited[(int)new_pos.x][(int)new_pos.y];
					min_dir = i;
				}
			}
		}

		cur_pos = (Vector2) { cur_pos.x + dirs[min_dir].x, cur_pos.y + dirs[min_dir].y };
		list_push(l, *object_new(object_types[0], (Vector2) { cur_pos.x, cur_pos.y }, 0));
	}

	return l;
}

list* flood_fill_search_start(game* g, Vector2 start, Vector2 goal) {
	list* l = list_new();

	// create visited grid
	int** visited = malloc(g->lvl->width * sizeof(int*));
	for (int i = 0; i < g->lvl->width; i++) {
		visited[i] = calloc(g->lvl->height, sizeof(int));
	}


	// flood fill primer
	visited[(int)goal.x][(int)goal.y] = 1;
	flood_fill_primer(visited, goal.x, goal.y, 1, g); // goal?

	// print primed floodfill
	/*printf("flood fill primed: \n");
	for (int x = 0; x < g->lvl->height; x++) {
		for (int y = 0; y < g->lvl->width; y++) {
			printf("%d\t", visited[y][x]);
		}
		printf("\n");
	}*/


	if (visited[(int)start.x][(int)start.y]) {
		//printf("prime found plr, searching\n");
		flood_fill_search(l, visited, start, goal);

		// print path
		/*for (int i = 0; i < l->length; i++) {
			Vector2 pos = list_get(l, i)->position;
			printf("go to %f, %f\n", pos.x, pos.y);
		}*/
	}

	// free visited grid
	for (int i = 0; i < g->lvl->width; i++) {
		free(visited[i]);
	}
	free(visited);

	return l;
}

float abs2(float a) {
	if (a < 0) {
		return -a;
	}
	return a;
}

void move_to_player(object* obj, game* g) {
	// flood fill
	//printf("this is the data for %s\n", obj->type.name);
	
	if (abs2(abs2((g->plr_position.x - obj->position.x)) + abs2((g->plr_position.y - obj->position.y))) <= 1) {
		//printf("we dont need to move here\n");
		//printf("attacked player\n");
		g->plr_health--;
	} else {
		list* flood = flood_fill_search_start(g, (Vector2) { g->plr_position.x, g->plr_position.y }, obj->position);

		if (flood->length >= 1) {
			Vector2 next_move = list_get(flood, flood->length - 2)->position;
			if (rand_int(0, 4) != 1) {
				// follow the player

				object* o = object_at(next_move.x, next_move.y);
				if (o == (void*)0) {
					g->lvl->data[(int)obj->position.x][(int)obj->position.y] = CELL_EMPTY;
					g->lvl->data[(int)next_move.x][(int)next_move.y] = CELL_OBJ;
					obj->position = (Vector2) { next_move.x, next_move.y };
				}

				//check_disconnected_cells();
			}
		}
	}

	
}