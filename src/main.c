#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "SDL_nmix.h"
#include "invaders.h"

#define JOYSTICK_DEAD_ZONE 8000

#define FILE1 "roms/invaders.h"
#define FILE2 "roms/invaders.g"
#define FILE3 "roms/invaders.f"
#define FILE4 "roms/invaders.e"
#define FILE_TEST1 "roms/invaders_test_rom/Sitest_716.bin"
#define FILE_TEST2 "roms/test.h"

static SDL_Renderer* renderer = NULL;
static SDL_Texture* texture = NULL;
static SDL_Event e;

static invaders si;

static bool should_quit = false;
static int speed = 1;
static uint32_t current_time = 0;
static uint32_t last_time = 0;
static uint32_t dt = 0;

static void update_screen(invaders* const si) {
  int pitch = 0;
  void* pixels = NULL;
  if (SDL_LockTexture(texture, NULL, &pixels, &pitch) != 0) {
    SDL_Log("Unable to lock texture: %s", SDL_GetError());
  } else {
    memcpy(pixels, si->screen_buffer, pitch * SCREEN_HEIGHT);
  }
  SDL_UnlockTexture(texture);
}

void mainloop(void) {
  current_time = SDL_GetTicks();
  dt = current_time - last_time;

  while (SDL_PollEvent(&e) != 0) {
    if (e.type == SDL_QUIT) {
      should_quit = true;
#ifdef __EMSCRIPTEN__
      emscripten_exit_with_live_runtime();
#endif
    } else if (e.type == SDL_KEYDOWN) {
      SDL_Scancode key = e.key.keysym.scancode;
      if (key == SDL_SCANCODE_C) {
        si.port1 |= 1 << 0; // coin
      } else if (key == SDL_SCANCODE_2) {
        si.port1 |= 1 << 1; // P2 start button
      } else if (key == SDL_SCANCODE_RETURN) {
        si.port1 |= 1 << 2; // P1 start button
      } else if (key == SDL_SCANCODE_SPACE) {
        si.port1 |= 1 << 4; // P1 shoot button
        si.port2 |= 1 << 4; // P2 shoot button
      } else if (key == SDL_SCANCODE_LEFT) {
        si.port1 |= 1 << 5; // P1 joystick left
        si.port2 |= 1 << 5; // P2 joystick left
      } else if (key == SDL_SCANCODE_RIGHT) {
        si.port1 |= 1 << 6; // P1 joystick right
        si.port2 |= 1 << 6; // P2 joystick right
      } else if (key == SDL_SCANCODE_T) {
        si.port2 |= 1 << 2; // tilt
      } else if (key == SDL_SCANCODE_F9) { // to toggle between b&w / color
        si.colored_screen = !si.colored_screen;
      } else if (key == SDL_SCANCODE_ESCAPE) {
// allow web users to kill game with esc key
#ifdef __EMSCRIPTEN__
        SDL_Event quit_event;
        quit_event.type = SDL_QUIT;
        SDL_PushEvent(&quit_event);
#endif
      } else if (key == SDL_SCANCODE_TAB) {
        speed = 5;
      }
    } else if (e.type == SDL_KEYUP) {
      SDL_Scancode key = e.key.keysym.scancode;
      if (key == SDL_SCANCODE_C) {
        si.port1 &= 0b11111110; // coin
      } else if (key == SDL_SCANCODE_2) {
        si.port1 &= 0b11111101; // P2 start button
      } else if (key == SDL_SCANCODE_RETURN) {
        si.port1 &= 0b11111011; // P1 start button
      } else if (key == SDL_SCANCODE_SPACE) {
        si.port1 &= 0b11101111; // P1 shoot button
        si.port2 &= 0b11101111; // P2 shoot button
      } else if (key == SDL_SCANCODE_LEFT) {
        si.port1 &= 0b11011111; // P1 joystick left
        si.port2 &= 0b11011111; // P2 joystick left
      } else if (key == SDL_SCANCODE_RIGHT) {
        si.port1 &= 0b10111111; // P1 joystick right
        si.port2 &= 0b10111111; // P2 joystick right
      } else if (key == SDL_SCANCODE_T) {
        si.port2 &= 0b11111011; // tilt
      } else if (key == SDL_SCANCODE_TAB) {
        speed = 1;
        // clear the queued audio to avoid audio delays
        // SDL_ClearQueuedAudio(audio_device); // @TODO
      }
    } else if (e.type == SDL_JOYAXISMOTION) {
      if (e.jaxis.axis == 0) { // x axis
        if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
          si.port1 |= 1 << 5; // P1 joystick left
          si.port2 |= 1 << 5; // P2 joystick left
        } else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
          si.port1 |= 1 << 6; // P1 joystick right
          si.port2 |= 1 << 6; // P2 joystick right
        } else {
          si.port1 &= 0b11011111; // P1 joystick left
          si.port2 &= 0b11011111; // P2 joystick left

          si.port1 &= 0b10111111; // P1 joystick right
          si.port2 &= 0b10111111; // P2 joystick right
        }
      }
    } else if (e.type == SDL_JOYBUTTONDOWN) {
      if (e.jbutton.button == 1) { // B
        si.port1 |= 1 << 0; // coin
      } else if (e.jbutton.button == 0) {
        si.port1 |= 1 << 4; // P1 shoot button
        si.port2 |= 1 << 4; // P2 shoot button
      } else if (e.jbutton.button == 8) { // start
        si.port1 |= 1 << 2; // P1 start button
      } else if (e.jbutton.button == 9) { // select
        si.port1 |= 1 << 1; // P2 start button
      } else if (e.jbutton.button == 13) {
        si.port1 |= 1 << 5; // P1 joystick left
        si.port2 |= 1 << 5; // P2 joystick left
      } else if (e.jbutton.button == 14) {
        si.port1 |= 1 << 6; // P1 joystick right
        si.port2 |= 1 << 6; // P2 joystick right
      } else if (e.jbutton.button == 4) { // LB
        // to toggle between b&w / color
        si.colored_screen = !si.colored_screen;
      }
    } else if (e.type == SDL_JOYBUTTONUP) {
      if (e.jbutton.button == 1) { // B
        si.port1 &= 0b11111110; // coin
      } else if (e.jbutton.button == 0) {
        si.port1 &= 0b11101111; // P1 shoot button
        si.port2 &= 0b11101111; // P2 shoot button
      } else if (e.jbutton.button == 8) { // start
        si.port1 &= 0b11111011; // P1 start button
      } else if (e.jbutton.button == 9) { // select
        si.port1 &= 0b11111101; // P2 start button
      } else if (e.jbutton.button == 13) {
        si.port1 &= 0b11011111; // P1 joystick left
        si.port2 &= 0b11011111; // P2 joystick left
      } else if (e.jbutton.button == 14) {
        si.port1 &= 0b10111111; // P1 joystick right
        si.port2 &= 0b10111111; // P2 joystick right
      }
    }
  }

  invaders_update(&si, dt * speed);

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  last_time = current_time;
}

int main(void) {
  // SDL init
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0) {
    SDL_Log("unable to initialize SDL: %s", SDL_GetError());
    return 1;
  }

  if (Sound_Init() == 0) {
    SDL_Log("Unable to initialise SDL_sound: %s", Sound_GetError());
    return 1;
  }

  if (NMIX_OpenAudio(NMIX_DEFAULT_DEVICE, NMIX_DEFAULT_FREQUENCY,
          NMIX_DEFAULT_SAMPLES) != 0) {
    SDL_Log("NMIX Error: %s\n", SDL_GetError());
    return -1;
  }
  NMIX_SetMasterGain(.5);

  // create SDL window
  SDL_Window* window = SDL_CreateWindow("Space Invaders",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH * 2,
      SCREEN_HEIGHT * 2, SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

  if (window == NULL) {
    SDL_Log("unable to create window: %s", SDL_GetError());
    return 1;
  }

  SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
  // SDL_ShowCursor(SDL_DISABLE);

  // create renderer
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (renderer == NULL) {
    SDL_Log("unable to create renderer: %s", SDL_GetError());
    return 1;
  }

  SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

  // print info on renderer:
  SDL_RendererInfo renderer_info;
  SDL_GetRendererInfo(renderer, &renderer_info);
  SDL_Log("using renderer %s", renderer_info.name);

  // create texture
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
      SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

  if (texture == NULL) {
    SDL_Log("unable to create texture: %s", SDL_GetError());
    return 1;
  }

  // joystick init
  SDL_Joystick* joystick = NULL;
  if (SDL_NumJoysticks() > 0) {
    joystick = SDL_JoystickOpen(0);

    if (joystick) {
      SDL_Log("opened joystick 0");
    } else {
      SDL_LogError(SDL_LOG_CATEGORY_ERROR, "opening joystick 0");
    }
  }

  // game init
  invaders_init(&si);
  si.update_screen = update_screen;
  update_screen(&si);

  // loading roms
  if (invaders_load_rom(&si, FILE1, 0x0000) != 0)
    return 1;
  if (invaders_load_rom(&si, FILE2, 0x0800) != 0)
    return 1;
  if (invaders_load_rom(&si, FILE3, 0x1000) != 0)
    return 1;
  if (invaders_load_rom(&si, FILE4, 0x1800) != 0)
    return 1;

// main loop
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(mainloop, 0, 1);
#else
  while (!should_quit) {
    mainloop();
  }
#endif

  if (joystick) {
    SDL_JoystickClose(joystick);
  }

  NMIX_CloseAudio();
  Sound_Quit();
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
