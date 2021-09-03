# Space Invaders

A Space Invaders emulator written in C99 with the SDL2.

![space invaders demo gif](demo.gif)

Features:

- [x] full emulation
- [x] sound
- [x] colourised screen (can also play in black & white)
- [x] two-players mode
- [x] joystick support
- [x] web export to HTML5 using emscripten
- [x] high score automatically saved

## How to build it

You should be able to launch it by running:

```bash
mkdir build
cd build
cmake .. -DROMS_DIR=/path/to/roms
make
./invaders
```

It has been tested on macOS 10.13 with clang and debian 8 with gcc 5.

You'll need to have the files `invaders.e`, `invaders.f`, `invaders.g` and `invaders.h`. You can also drop the Space Invaders wav files in the same folder if you have them.

You can also build it for the web if you have emscripten installed (for macOS, install with `brew install emscripten`). Just replace `cmake ..` with `emcmake cmake ..` to obtain four files invaders.(js|wasm|data|html) which can be hosted on a web server.

## How to play

| Computer | Space Invaders                                 |
| -------- | ---------------------------------------------- |
| c        | insert a coin                                  |
| return   | start a game                                   |
| 2        | start a game in two-player mode                |
| left     | move the player left                           |
| right    | move the player right                          |
| space    | shoot                                          |
| t        | tilt the machine                               |
| F9       | toggle between black and white / coloured mode |
