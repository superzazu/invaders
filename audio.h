#ifndef AUDIO_H
#define AUDIO_H

// play sounds files with SDL2_mixer
// example use:
//     audio_init();
//     int my_sound = audio_load_snd("mysoundfile.wav");
//     audio_play_snd(my_sound);
//     audio_quit(); // auto frees the buffered sound files

#include <SDL.h>
#include <SDL_mixer.h>

#define MAX_SOUNDS 256

void audio_init();
void audio_quit();

int audio_load_snd(const char* filename);
int audio_play_snd(const int id);
void audio_free_snd(const int id);

// TODO: music functions?

#endif  // AUDIO_H