#include <SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "invaders.h"
#include "audio.h"

#define JOYSTICK_DEAD_ZONE 8000

#define FILE1 "res/roms/invaders.h"
#define FILE2 "res/roms/invaders.g"
#define FILE3 "res/roms/invaders.f"
#define FILE4 "res/roms/invaders.e"
#define FILE_TEST1 "res/roms/invaders_test_rom/Sitest_716.bin"
#define FILE_TEST2 "res/roms/test.h"

static bool should_quit = false;
static SDL_Event e;
static u32 timer = 0;
static invaders si;
static SDL_Renderer* renderer;
static SDL_Texture* texture = NULL;

static void update_screen(invaders* const si) {
    const uint32_t pitch = sizeof(uint8_t) * 4 * SCREEN_WIDTH;
    SDL_UpdateTexture(texture, NULL, &si->screen_buffer, pitch);
}

void mainloop(void) {
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            should_quit = true;
            #ifdef __EMSCRIPTEN__
            emscripten_exit_with_live_runtime();
            #endif
        }
        else if (e.type == SDL_KEYDOWN) {
            const u32 key = e.key.keysym.sym;
            if (key == SDLK_c) {
                si.port1 |= 1 << 0; // coin
            }
            else if (key == SDLK_2) {
                si.port1 |= 1 << 1; // P2 start button
            }
            else if (key == SDLK_RETURN) {
                si.port1 |= 1 << 2; // P1 start button
            }
            else if (key == SDLK_SPACE) {
                si.port1 |= 1 << 4; // P1 shoot button
                si.port2 |= 1 << 4; // P2 shoot button
            }
            else if (key == SDLK_LEFT) {
                si.port1 |= 1 << 5; // P1 joystick left
                si.port2 |= 1 << 5; // P2 joystick left
            }
            else if (key == SDLK_RIGHT) {
                si.port1 |= 1 << 6; // P1 joystick right
                si.port2 |= 1 << 6; // P2 joystick right
            }
            else if (key == SDLK_t) {
                si.port2 |= 1 << 2; // tilt
            }
            else if (key == SDLK_F9) { // to toggle between b&w / color
                si.colored_screen = !si.colored_screen;
            }
            else if (key == SDLK_ESCAPE) {
                // allow web users to kill game with esc key
                #ifdef __EMSCRIPTEN__
                SDL_Event quit_event;
                quit_event.type = SDL_QUIT;
                SDL_PushEvent(&quit_event);
                #endif
            }
        }
        else if (e.type == SDL_KEYUP) {
            const u32 key = e.key.keysym.sym;
            if (key == SDLK_c) {
                si.port1 &= 0b11111110; // coin
            }
            else if (key == SDLK_2) {
                si.port1 &= 0b11111101; // P2 start button
            }
            else if (key == SDLK_RETURN) {
                si.port1 &= 0b11111011; // P1 start button
            }
            else if (key == SDLK_SPACE) {
                si.port1 &= 0b11101111; // P1 shoot button
                si.port2 &= 0b11101111; // P2 shoot button
            }
            else if (key == SDLK_LEFT) {
                si.port1 &= 0b11011111; // P1 joystick left
                si.port2 &= 0b11011111; // P2 joystick left
            }
            else if (key == SDLK_RIGHT) {
                si.port1 &= 0b10111111; // P1 joystick right
                si.port2 &= 0b10111111; // P2 joystick right
            }
            else if (key == SDLK_t) {
                si.port2 &= 0b11111011; // tilt
            }
        }
        else if (e.type == SDL_JOYAXISMOTION) {
            if (e.jaxis.axis == 0) { // x axis
                if (e.jaxis.value < -JOYSTICK_DEAD_ZONE) {
                    si.port1 |= 1 << 5; // P1 joystick left
                    si.port2 |= 1 << 5; // P2 joystick left
                }
                else if (e.jaxis.value > JOYSTICK_DEAD_ZONE) {
                    si.port1 |= 1 << 6; // P1 joystick right
                    si.port2 |= 1 << 6; // P2 joystick right
                }
                else {
                    si.port1 &= 0b11011111; // P1 joystick left
                    si.port2 &= 0b11011111; // P2 joystick left

                    si.port1 &= 0b10111111; // P1 joystick right
                    si.port2 &= 0b10111111; // P2 joystick right
                }
            }
        }
        else if (e.type == SDL_JOYBUTTONDOWN) {
            if (e.jbutton.button == 1) { // B
                si.port1 |= 1 << 0; // coin
            }
            else if (e.jbutton.button == 0) {
                si.port1 |= 1 << 4; // P1 shoot button
                si.port2 |= 1 << 4; // P2 shoot button
            }
            else if (e.jbutton.button == 8) { // start
                si.port1 |= 1 << 2; // P1 start button
            }
            else if (e.jbutton.button == 9) { // select
                si.port1 |= 1 << 1; // P2 start button
            }
            else if (e.jbutton.button == 13) {
                si.port1 |= 1 << 5; // P1 joystick left
                si.port2 |= 1 << 5; // P2 joystick left
            }
            else if (e.jbutton.button == 14) {
                si.port1 |= 1 << 6; // P1 joystick right
                si.port2 |= 1 << 6; // P2 joystick right
            }
            else if (e.jbutton.button == 4) { // LB
                // to toggle between b&w / color
                si.colored_screen = !si.colored_screen;
            }
        }
        else if (e.type == SDL_JOYBUTTONUP) {
            if (e.jbutton.button == 1) { // B
                si.port1 &= 0b11111110; // coin
            }
            else if (e.jbutton.button == 0) {
                si.port1 &= 0b11101111; // P1 shoot button
                si.port2 &= 0b11101111; // P2 shoot button
            }
            else if (e.jbutton.button == 8) { // start
                si.port1 &= 0b11111011; // P1 start button
            }
            else if (e.jbutton.button == 9) { // select
                si.port1 &= 0b11111101; // P2 start button
            }
            else if (e.jbutton.button == 13) {
                si.port1 &= 0b11011111; // P1 joystick left
                si.port2 &= 0b11011111; // P2 joystick left
            }
            else if (e.jbutton.button == 14) {
                si.port1 &= 0b10111111; // P1 joystick right
                si.port2 &= 0b10111111; // P2 joystick right
            }
        }
    }

    // update the game for each frame (every 1/60 seconds)
    if (SDL_GetTicks() - timer > (1.f / FPS) * 1000) {
        timer = SDL_GetTicks();
        invaders_update(&si);
        invaders_gpu_update(&si);
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0) {
        SDL_Log("unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    // create SDL window
    SDL_Window* window = SDL_CreateWindow("Space Invaders",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH * 2,
        SCREEN_HEIGHT * 2,
        SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        SDL_Log("unable to create window: %s", SDL_GetError());
        return 1;
    }

    SDL_SetWindowMinimumSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_ShowCursor(SDL_DISABLE);

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
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT);

    if (texture == NULL) {
        SDL_Log("unable to create texture: %s", SDL_GetError());
        return 1;
    }

    // joystick init
    SDL_Joystick *joystick = NULL;
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0);

        if (joystick) {
            SDL_Log("opened joystick 0");
        }
        else {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "opening joystick 0");
        }
    }

    // audio init
    audio_init();

    // game init
    invaders_init(&si);
    si.update_screen = update_screen;
    update_screen(&si);

    // loading roms
    if (invaders_load_rom(&si, FILE1, 0x0000) != 0) return 1;
    if (invaders_load_rom(&si, FILE2, 0x0800) != 0) return 1;
    if (invaders_load_rom(&si, FILE3, 0x1000) != 0) return 1;
    if (invaders_load_rom(&si, FILE4, 0x1800) != 0) return 1;

    // main loop
    timer = SDL_GetTicks();
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

    audio_quit();
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
