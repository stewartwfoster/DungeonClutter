#pragma once

#include "objects.h"

#define LIST_DEFAULT_SIZE 8

typedef struct list {
	object* data;
	int size;
	int length;
} list;

list* list_new();
object* list_get(list* list, int pos);
void list_set(list* list, int pos, object value);
void list_push(list* list, object value);
object* list_remove(list* list, int pos);
object* list_pop(list* list);
void list_free(list* list);
void list_print(list* list);