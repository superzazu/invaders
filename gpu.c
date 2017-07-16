#include "gpu.h"

void gpu_init(i8080* m) {
    // initializing the screen buffer with zeroes
    for (int x=0; x<256; x++) {
        for (int y=0; y<224; y++) {
            m->screen_buffer[y][x][0] = 0;
            m->screen_buffer[y][x][1] = 0;
            m->screen_buffer[y][x][2] = 0;
        }
    }

    // creating the texture
    glGenTextures(1, &m->texture);
    glBindTexture(GL_TEXTURE_2D, m->texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 224, 0,
                 GL_RGB, GL_FLOAT, &m->screen_buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_2D);
}

void gpu_update(i8080* m) {
    const int start_addr = 0x2400;

    // the screen is 256 * 224 pixels, and 1 byte contains 8 pixels
    for (int i = 0; i < 256 * 224 / 8; i++) {
        const int y = i * 8 / 256;
        const int base_x = (i * 8) % 256;
        const u8 cur_byte = m->memory[start_addr + i];

        for (u8 bit = 0; bit < 8; bit++) {
            const int px = base_x + bit;
            const int py = y;
            const bool is_pixel_lit = (cur_byte >> bit) & 1;
            float r = 0, g = 0, b = 0;

            if (is_pixel_lit) {
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

            m->screen_buffer[py][px][0] = r;
            m->screen_buffer[py][px][1] = g;
            m->screen_buffer[py][px][2] = b;
        }
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 224, 0,
                 GL_RGB, GL_FLOAT, &m->screen_buffer);
}

void gpu_draw(i8080* m) {
    glBegin(GL_QUADS);
    glTexCoord2f(1, 0); glVertex2f(-1, 1);
    glTexCoord2f(0, 0); glVertex2f(-1, -1);
    glTexCoord2f(0, 1); glVertex2f(1, -1);
    glTexCoord2f(1, 1); glVertex2f(1, 1);
    glEnd();
}
