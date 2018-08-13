#include "list.h"
#include <stdio.h>

list* list_new() {
	list* l = malloc(sizeof(list));
	l->length = 0;
	l->size = LIST_DEFAULT_SIZE;
	l->data = malloc(l->size * sizeof(object));

	return l;
}

void list_free(list* list) {
	/*list_print(list);
	for (int i = 0; i < list->length; i++) {
		object* o = list_get(list, i);
		printf("freeing %s\n", o->type.name);
		free(o);
	}*/

	free(list->data);
	free(list);
}

void list_increase(list* list) {
	list->size *= 2;
	list->data = realloc(list->data, list->size * sizeof(object));
}

object* list_get(list* list, int pos) {
	return &list->data[pos];
}

void list_set(list* list, int pos, object value) {
	if (pos < list->size) {
		list->data[pos] = value;
	} else {
		list_increase(list);
		list_set(list, pos, value);
	}
}

void list_push(list* list, object value) {
	if (list->length < list->size) {
		list->data[list->length++] = value;
	} else {
		list_increase(list);
		list_push(list, value);
	}
}

object* list_remove(list* list, int pos) {
	object* val = list_get(list, pos);
	for (int i = pos; i < list->length - 1; i++) {
		list->data[i] = list->data[i + 1];
	}
	list->length--;
	return val;
}

object* list_pop(list* list) {
	return list_remove(list, list->length - 1);
}

void list_print(list* list) {
	for (int i = 0; i < list->length; i++) {
		printf("%d :: %s\n", i, list->data[i].type.name);
	}
}