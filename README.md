# Space Invaders

A Space Invaders emulator, written in C with SDL2 (using OpenGL 2), and SDL2_mixer for sound.

![space invaders demo gif](demo.gif)

Features:

- [x] full emulation
- [x] sound
- [x] colourised screen (can also play in black & white)
- [x] two-players mode
- [x] joystick support

To run it: `make && ./invaders`. Keys are: `c` to insert a coin, `enter` to start a game (`2` for two-player mode), `left` / `right` to move, and `space` to shoot. You can also tilt the machine with `t` and press `F9` to toggle between black and white/coloured screen.

You'll need to have the files `invaders.e`, `invaders.f`, `invaders.g` and `invaders.h` in the `roms` folder. You can also drop the Space Invaders wav files in `roms/snd` if you have them.

To build the .app bundle for macOS, you need to have [SDL2.framework](http://libsdl.org/release/SDL2-2.0.8.dmg) and [SDL2_mixer.framework](https://www.libsdl.org/projects/SDL_mixer/release/SDL2_mixer-2.0.2.dmg) file in `/Library/Frameworks` folder. Then you can open the .xcodeproj file and build the project.
