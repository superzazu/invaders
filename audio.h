#ifndef AUDIO_H
#define AUDIO_H

// play sounds files with SDL2_mixer
// example use:
//     audio_init();
//     int my_sound = audio_load_snd("mysoundfile.wav");
//     audio_play_snd(my_sound);
//     audio_quit(); // auto frees the buffered sound files

#include <SDL.h>

#if __EMSCRIPTEN__
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
#endif

#define MAX_SOUNDS 256

int audio_init(void);
void audio_quit(void);

int audio_load_snd(const char* filename);
int audio_play_snd(int id);
int audio_volume_snd(int id, int vol);
void audio_free_snd(int id);

// TODO: music functions?

#endif  // AUDIO_H
