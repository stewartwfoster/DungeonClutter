#include "sounds.h"
#include "utils.h"

Sound sound_load(const char* filename) {
	char* fp = get_file_path(filename);
	Sound sound = LoadSound(fp);
	free(fp);

	return sound;
}

void sounds_init() {
	snd_move1 = sound_load("resources\\move1.wav");
	snd_menu1 = sound_load("resources\\menu_select1.wav");
	snd_menu2 = sound_load("resources\\menu_select2.wav");
	snd_explode = sound_load("resources\\explode1.wav");
}

void sounds_unload() {
	UnloadSound(snd_move1);
	UnloadSound(snd_menu1);
	UnloadSound(snd_menu2);
	UnloadSound(snd_explode);
}