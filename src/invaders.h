#ifndef INVADERS_INVADERS_H
#define INVADERS_INVADERS_H

#include <string.h>
#include <SDL.h>

#include "SDL_nmix.h"
#include "SDL_nmix_file.h"

#include "8080/i8080.h"

#define SCREEN_WIDTH 224
#define SCREEN_HEIGHT 256
#define FPS 59.541985
#define CLOCK_SPEED 1996800
#define CYCLES_PER_FRAME (CLOCK_SPEED / FPS)

#define VRAM_ADDR 0x2400

typedef struct invaders invaders;
struct invaders {
  i8080 cpu;
  u8 memory[0x10000];

  u8 next_interrupt;
  bool colored_screen;
  NMIX_FileSource* sounds[9];

  // SI-specific ports & shift registers that are used in IN/OUT opcodes
  u8 port1, port2;
  u8 shift_msb, shift_lsb, shift_offset;
  u8 last_out_port3, last_out_port5;

  // screen pixel buffer
  u8 screen_buffer[SCREEN_HEIGHT][SCREEN_WIDTH][4];
  // function pointer provided by the user that will be called every time
  // the screen must be updated:
  void (*update_screen)(invaders* const si);
};

void invaders_init(invaders* const si);
void invaders_update(invaders* const si, int ms);
void invaders_gpu_update(invaders* const si);
void invaders_play_sound(invaders* const si, u8 bank);
int invaders_load_rom(invaders* const si, const char* filename, u16 start_addr);

#endif // INVADERS_INVADERS_H
