#ifndef SPACEI_INVADERS_H
#define SPACEI_INVADERS_H

#include <string.h>
#include <SDL.h>
#include <SDL_opengl.h>

#include "audio.h"

#include "8080/types.h"
#include "8080/i8080.h"

#define WIN_WIDTH 224
#define WIN_HEIGHT 256
#define FPS 60

#define CYCLES_PER_FRAME 2000000 / 60 // 2Mhz at 60 fps
#define HALF_CYCLES_PER_FRAME CYCLES_PER_FRAME / 2

typedef struct invaders {
    // u8 memory[0x10000];
    i8080 cpu;

    u8 next_interrupt;
    float screen_buffer[224][256][3];
    GLuint texture;
    u8 port1, port2;
    u8 shift0, shift1, shift_offset;
    u8 last_out_port3, last_out_port5;
    bool colored_screen;
    int sounds[9];
} invaders;

void invaders_init(invaders* const si);
void invaders_update(invaders* const si);

void invaders_gpu_init(invaders* const si);
void invaders_gpu_update(invaders* const si);
void invaders_gpu_draw(invaders* const si);

void invaders_play_sound(invaders* const si, const u8 bank);

// memory handling
static u8 memory[0x10000] = {0};
u8 invaders_rb(const u16 addr);
void invaders_wb(const u16 addr, const u8 val);
int invaders_load_rom(const char* filename, const u16 start_addr);

#endif  // SPACEI_INVADERS_H
