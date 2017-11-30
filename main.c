#include <SDL.h>
#include <SDL_opengl.h>

#include "8080/types.h"
#include "invaders.h"
#include "audio.h"

static invaders si;

int main(int argc, char **argv) {
    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    // create SDL window
    SDL_Window* window = SDL_CreateWindow("Space Invaders",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIN_WIDTH * 2,
        WIN_HEIGHT * 2,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (window == NULL) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        return 1;
    }

    // create OpenGL context
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);

    if (gl_context == NULL) {
        SDL_Log("Unable to create OpenGL context: %s", SDL_GetError());
        return 1;
    }

    // set vsync
    if (SDL_GL_SetSwapInterval(1) < 0) {
        SDL_Log("Unable to set vsync: %s", SDL_GetError());
    }

    // OpenGL init
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glMatrixMode(GL_MODELVIEW);
    // glLoadIdentity();
    glClearColor(0.f, 0.f, 0.f, 1.f);
    glViewport(0, 0, WIN_WIDTH * 2, WIN_HEIGHT * 2);

    // audio init
    audio_init();

    // game init
    invaders_init(&si);
    invaders_gpu_init(&si);

    char *base_path = SDL_GetBasePath();
    char *file1 = "roms/invaders.h";
    char *file2 = "roms/invaders.g";
    char *file3 = "roms/invaders.f";
    char *file4 = "roms/invaders.e";
    char *file_test1 = "roms/invaders_test_rom/Sitest_716.bin";
    char *file_test2 = "roms/test.h";

    int BUF_SIZE = strlen(base_path) + strlen(file_test1) + 1;
    char *full_path = malloc(BUF_SIZE);

    snprintf(full_path, BUF_SIZE, "%s%s", base_path, file1);
    if (invaders_load_rom(full_path, 0x0000) != 0) return 1;

    snprintf(full_path, BUF_SIZE, "%s%s", base_path, file2);
    if (invaders_load_rom(full_path, 0x0800) != 0) return 1;

    snprintf(full_path, BUF_SIZE, "%s%s", base_path, file3);
    if (invaders_load_rom(full_path, 0x1000) != 0) return 1;

    snprintf(full_path, BUF_SIZE, "%s%s", base_path, file4);
    if (invaders_load_rom(full_path, 0x1800) != 0) return 1;

    // test rom
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, file_test2);
    // if (invaders_load_rom(full_path, 0x0000) != 0) return 1;

    free(full_path);
    free(base_path);

    // main loop
    bool quit = false;
    SDL_Event e;
    u32 timer = SDL_GetTicks();

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_WINDOWEVENT &&
                     e.window.event == SDL_WINDOWEVENT_RESIZED) {
                glViewport(0, 0, e.window.data1, e.window.data2);
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
        }

        // update the game for each frame (every 1/60 seconds)
        if (SDL_GetTicks() - timer > (1 / FPS) * 1000) {
            timer = SDL_GetTicks();
            invaders_update(&si);
            invaders_gpu_update(&si);
        }

        // render
        glClear(GL_COLOR_BUFFER_BIT);
        invaders_gpu_draw(&si);

        SDL_GL_SwapWindow(window);
    }

    audio_quit();
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_Quit();

    return 0;
}
