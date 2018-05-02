# Space Invaders

A Space Invaders emulator, written in C with the SDL2 and SDL2_mixer for sound.

![space invaders demo gif](demo.gif)

Features:

- [x] full emulation
- [x] sound
- [x] colourised screen (can also play in black & white)
- [x] two-players mode
- [x] joystick support

## How to run it

You should be able to launch it by running `make && ./invaders`. It has been tested on macOS 10.13 with clang and debian 8 with gcc 5.
You'll need to have the files `invaders.e`, `invaders.f`, `invaders.g` and `invaders.h` in the `roms` folder. You can also drop the Space Invaders wav files in `snd` if you have them.

To build the .app bundle for macOS, you need to have [SDL2.framework](http://libsdl.org/release/SDL2-2.0.8.dmg) and [SDL2_mixer.framework](https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.2.dmg) file in `/Library/Frameworks` folder. Then you can open the .xcodeproj file and build the project.

## How to play

Computer | Space Invaders
--- | ---
c | insert a coin
return | start a game
2 | start a game in two-player mode
left | move the player left
right | move the player right
space | shoot
t | tilt the machine
F9 | toggle between black and white / coloured mode
