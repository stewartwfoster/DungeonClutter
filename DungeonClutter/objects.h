#pragma once

#include "raylib.h"

#define NUM_OBJECT_TYPES 8
#define NUM_STATIC_TYPES 6
#define NUM_PROTECTED_TYPES 2

typedef enum interact_result {
	INTERACT_NONE, INTERACT_DELETE, INTERACT_CONTINUE
} interact_result;

typedef struct animation {
	Texture2D* textures;
	int num_textures;
	int animation_time;
} animation;

typedef struct object_type {
	const char* name;
	animation anim;
	int health;
	interact_result(*interact)(void*); // interact(object*)
	void(*step)(void*, void*); // step(object*)
} object_type;

typedef struct object {
	object_type type;
	Vector2 position;
	int health;
	int max_health;
	int damaged; // when damaged, start at ticks, remove each tick damage is shown
} object;

void objects_init();
object* object_new(object_type type, Vector2 position, int health);
void object_free(object* obj);

extern object_type object_types[NUM_OBJECT_TYPES];



