#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "main.h"

const char* get_file_path(const char* filename) {
	char* buffer = malloc(1000 * sizeof(char));
	snprintf(buffer, 1000, "%s\\..\\%s", exe_dir, filename);
	return buffer;
}

int rand_int(int min, int max) {
	return rand() % (max + 1 - min) + min;
}