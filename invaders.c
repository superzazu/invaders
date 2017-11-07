#include "invaders.h"

void invaders_init(invaders* const si) {
    memset(memory, 0, sizeof memory);
    i8080_init(&si->cpu);
    si->cpu.read_byte = invaders_rb;
    si->cpu.write_byte = invaders_wb;
    si->next_interrupt = 0x08;
    si->port1 = 0;
    si->port2 = 0;
    si->shift0 = 0;
    si->shift1 = 0;
    si->shift_offset = 0;
    si->last_out_port3 = 0;
    si->last_out_port5 = 0;
    si->colored_screen = true;

    // load sounds
    char* base_path = SDL_GetBasePath();
    const int BUF_SIZE = strlen(base_path) + strlen("roms/snd/XX.wav") + 1;
    char* full_path = malloc(BUF_SIZE);

    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/8.wav");
    si->sounds[0] = audio_load_snd(full_path); // ufo sound
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/1.wav");
    si->sounds[1] = audio_load_snd(full_path); // shoot sound
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/2.wav");
    si->sounds[2] = audio_load_snd(full_path); // player die
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/3.wav");
    si->sounds[3] = audio_load_snd(full_path); // alien die
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/4.wav");
    si->sounds[4] = audio_load_snd(full_path); // alien move 1
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/5.wav");
    si->sounds[5] = audio_load_snd(full_path); // alien move 2
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/6.wav");
    si->sounds[6] = audio_load_snd(full_path); // alien move 3
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/7.wav");
    si->sounds[7] = audio_load_snd(full_path); // alien move 4
    snprintf(full_path, BUF_SIZE, "%s%s", base_path, "roms/snd/10.wav");
    si->sounds[8] = audio_load_snd(full_path); // ufo hit

    free(full_path);
    free(base_path);
}

void invaders_update(invaders* const si) {
    // emulates the correct number of cycles for one frame
    // function to execute every 1/60s
    u32 count_cycles = 0;

    while (count_cycles <= CYCLES_PER_FRAME) {
        u32 start_cyc = si->cpu.cyc;
        const u8 opcode = i8080_rb(&si->cpu, si->cpu.pc);

        i8080_step(&si->cpu);

        count_cycles += si->cpu.cyc - start_cyc;

        // Space Invaders special opcodes (IN/OUT)
        switch (opcode) {
        case 0xDB: { // IN
            const u8 port = i8080_next_byte(&si->cpu);
            u8 value = 0;

            if (port == 1) { // button presses
                value = si->port1;
            }
            else if (port == 2) { // game settings
                value = si->port2;
            }
            else if (port == 3) {
                const u16 v = (si->shift1 << 8) | si->shift0;
                value = (v >> (8 - si->shift_offset)) & 0xFF;
            }
            else {
                fprintf(stderr, "error: unknown IN port %02X\n", port);
            }
            si->cpu.a = value;
        } break;
        case 0xD3: { // OUT
            const u8 port = i8080_next_byte(&si->cpu);
            const u8 value = si->cpu.a;

            if (port == 2) {
                si->shift_offset = value & 0x7;
            }
            else if (port == 3) {
                invaders_play_sound(si, 1);
            }
            else if (port == 4) {
                si->shift0 = si->shift1;
                si->shift1 = value;
            }
            else if (port == 5) {
                invaders_play_sound(si, 2);
            }
            else if (port == 6) {
                // unused port (debug port?)
            }
            else {
                fprintf(stderr, "error: unknown OUT port %02X\n", port);
            }
        } break;
        }

        // interrupts handling: every HALF_CYCLES_PER_FRAME cycles, an
        // interrupt is generated (0x08 and 0x10) if the iff == 1
        if (si->cpu.cyc >= HALF_CYCLES_PER_FRAME) {
            if (si->cpu.iff) {
                // generate interrupt
                si->cpu.iff = 0;
                i8080_call(&si->cpu, si->next_interrupt);
            }
            si->cpu.cyc -= HALF_CYCLES_PER_FRAME;
            si->next_interrupt = si->next_interrupt == 0x08 ? 0x10 : 0x08;
        }
    }
}

void invaders_gpu_init(invaders* const si) {
    // initialising the screen buffer with zeroes
    for (int x=0; x<256; x++) {
        for (int y=0; y<224; y++) {
            si->screen_buffer[y][x][0] = 0;
            si->screen_buffer[y][x][1] = 0;
            si->screen_buffer[y][x][2] = 0;
        }
    }

    // creating the texture
    glGenTextures(1, &si->texture);
    glBindTexture(GL_TEXTURE_2D, si->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 224, 0,
                 GL_RGB, GL_FLOAT, &si->screen_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
}

void invaders_gpu_update(invaders* const si) {
    const int start_addr = 0x2400;

    // the screen is 256 * 224 pixels, and 1 byte contains 8 pixels
    for (int i = 0; i < 256 * 224 / 8; i++) {
        const int y = i * 8 / 256;
        const int base_x = (i * 8) % 256;
        const u8 cur_byte = memory[start_addr + i];

        for (u8 bit = 0; bit < 8; bit++) {
            const int px = base_x + bit;
            const int py = y;
            const bool is_pixel_lit = (cur_byte >> bit) & 1;
            float r = 0, g = 0, b = 0;

            if (!si->colored_screen && is_pixel_lit) {
                r = 1; g = 1; b = 1;
            }
            else if (si->colored_screen && is_pixel_lit) {
                if (px < 16) {
                    if (py < 16 || py > 118 + 16) {
                        r = 1; g = 1; b = 1;
                    }
                    else {
                        g = 1;
                    }
                }
                else if (px >= 16 && px <= 16 + 56) {
                    g = 1;
                }
                else if (px >= 16 + 56 + 120 && px < 16 + 56 + 120 + 32) {
                    r = 1;
                }
                else {
                    r = 1; g = 1; b = 1;
                }
            }

            si->screen_buffer[py][px][0] = r;
            si->screen_buffer[py][px][1] = g;
            si->screen_buffer[py][px][2] = b;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 224, 0,
                 GL_RGB, GL_FLOAT, &si->screen_buffer);
}

void invaders_gpu_draw(invaders* const si) {
    glBegin(GL_QUADS);
    glTexCoord2f(1, 0); glVertex2f(-1, 1);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(0, 1); glVertex2f(1, -1);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glEnd();
}

void invaders_play_sound(invaders* const si, const u8 bank) {
    // plays a sound if the corresponding bit have changed from 0 to 1
    const u8 data = si->cpu.a;

    if (bank == 1) {
        if (data != si->last_out_port3) {
            if ((data & 0x1) && !(si->last_out_port3 & 0x1)) {
                audio_play_snd(si->sounds[0]);
            }
            if ((data & 0x2) && !(si->last_out_port3 & 0x2)) {
                audio_play_snd(si->sounds[1]);
            }
            if ((data & 0x4) && !(si->last_out_port3 & 0x4)) {
                audio_play_snd(si->sounds[2]);
            }
            if ((data & 0x8) && !(si->last_out_port3 & 0x8)) {
                audio_play_snd(si->sounds[3]);
            }

            si->last_out_port3 = data;
        }
    }
    else if (bank == 2) {
        if (data != si->last_out_port5) {
            if ((data & 0x1) && !(si->last_out_port5 & 0x1)) {
                audio_play_snd(si->sounds[4]);
            }
            if ((data & 0x2) && !(si->last_out_port5 & 0x2)) {
                audio_play_snd(si->sounds[5]);
            }
            if ((data & 0x4) && !(si->last_out_port5 & 0x4)) {
                audio_play_snd(si->sounds[6]);
            }
            if ((data & 0x8) && !(si->last_out_port5 & 0x8)) {
                audio_play_snd(si->sounds[7]);
            }
            if ((data & 0x10) && !(si->last_out_port5 & 0x10)) {
                audio_play_snd(si->sounds[8]);
            }

            si->last_out_port5 = data;
        }
    }
}

// memory handling
u8 invaders_rb(const u16 addr) {
    if (addr >= 0x4000) { // RAM mirror
        return memory[addr - 0x4000];
    }
    return memory[addr];
}

void invaders_wb(const u16 addr, const u8 val) {
    if (addr >= 0x4000) { // RAM mirror
        memory[addr - 0x4000] = val;
    }
    else {
        memory[addr] = val;
    }
}

int invaders_load_rom(const char* filename, const u16 start_addr) {
    FILE *f;
    long file_size = 0;

    f = fopen (filename, "rb");
    if (f == NULL) {
        fprintf("error: can't open ROM file '%s'\n", filename);
        return 1;
    }

    // obtain file size
    fseek(f, 0, SEEK_END);
    file_size = ftell(f);
    rewind(f);

    if (file_size > 0xFFFF) {
        fprintf("error: rom file '%s' too big to fit in memory\n", filename);
        return 1;
    }

    // copy file data to buffer
    u8 buffer[file_size];
    size_t result = fread(buffer, 1, file_size, f);
    if (result != file_size) {
        fprintf("error: while reading ROM file '%s'\n", filename);
        return 1;
    }

    // copy buffer to memory
    for (int i = 0; i < file_size; i++) {
        memory[start_addr + i] = buffer[i];
    }

    fclose(f);
    return 0;
}
