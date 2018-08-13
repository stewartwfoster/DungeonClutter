#pragma once

#define BGCOLOR (Color) {34, 39, 51, 255}
#define BGCOLOR_DARK (Color) {17, 19, 25, 255}

static const int WIDTH = 800, HEIGHT = 600;
static const char* TITLE = "Clutter Dungeon LD42";
static const int FPS = 60;

typedef enum gamestate {
	MENU, GAME, OPTIONS, CREDITS
} gamestate;