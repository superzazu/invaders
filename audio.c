#include "audio.h"

static Mix_Chunk* sounds[MAX_SOUNDS] = {NULL};

// initializes SDL_mixer
int audio_init(void) {
    const int chunk_size = 1024;

    int result = Mix_OpenAudio(
        MIX_DEFAULT_FREQUENCY,
        MIX_DEFAULT_FORMAT,
        MIX_DEFAULT_CHANNELS,
        chunk_size);

    if (result != 0) {
        SDL_Log("unable to initialize SDL_mixer: %s", Mix_GetError());
    }

    return result;
}

// frees buffered sounds and quits SDL_mixer
void audio_quit(void) {
    for (int i = 0; i < MAX_SOUNDS; i++) {
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
    for (int i = 0; i < MAX_SOUNDS; i++) {
        if (sounds[i] == NULL) {
            sounds[i] = Mix_LoadWAV(filename);
            if (sounds[i] == NULL) {
                SDL_Log("unable to load file '%s'\n", filename);
                return -1;
            }
            return i;
        }
    }
    SDL_Log("unable to load file (%d file limit reached)\n", MAX_SOUNDS);
    return -1;
}

// plays a WAV file
int audio_play_snd(int id) {
    if (id < 0 || id >= MAX_SOUNDS) {
        return 1;
    }
    return Mix_PlayChannel(-1, sounds[id], 0);
}

// sets the volume for a WAV file (between 0 and 128)
int audio_volume_snd(int id, int vol) {
    if (id < 0 || id >= MAX_SOUNDS) {
        return 1;
    }
    return Mix_VolumeChunk(sounds[id], vol);
}

// frees a WAV file from memory
void audio_free_snd(int id) {
    if (sounds[id] != NULL) {
        Mix_FreeChunk(sounds[id]);
        sounds[id] = NULL;
    }
}
