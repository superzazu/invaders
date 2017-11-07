#include "audio.h"

static Mix_Chunk *sounds[MAX_SOUNDS] = {0};

// initializes SDL_mixer
void audio_init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("Unable to initialize SDL_mixer: %s", Mix_GetError());
    }
}

// frees buffered sounds and quits SDL_mixer
void audio_quit() {
    for (int i=0; i < MAX_SOUNDS; i++) {
        if (sounds[i] != NULL) {
            audio_free_snd(i);
        }
    }
    Mix_Quit();
}

// loads a WAV file, and returns its id
// if function returned -1, an error occured
int audio_load_snd(const char* filename) {
    // loads a sound file then returns its id
    for (int i=0; i < MAX_SOUNDS; i++) {
        if (sounds[i] == NULL) {
            sounds[i] = Mix_LoadWAV(filename);
            if (sounds[i] == NULL) {
                SDL_Log("Unable to load file '%s'\n", filename);
                return -1;
            }
            return i;
        }
    }
    SDL_Log("Unable to load file (%d file limit reached)\n", MAX_SOUNDS);
    return -1;
}

// plays a WAV file (needs the id of the file)
int audio_play_snd(const int id) {
    if (id < 0 || id >= MAX_SOUNDS) {
        return 1;
    }
    return Mix_PlayChannel(-1, sounds[id], 0);
}

// frees a WAV file from memory
void audio_free_snd(const int id) {
    if (sounds[id] != NULL) {
        Mix_FreeChunk(sounds[id]);
        sounds[id] = NULL;
    }
}
