#include "gpu.h"

void gpu_draw(i8080* m) {
    const int start_addr = 0x2400;

    // the screen is 256 * 224 pixels, and 1 byte contains 8 pixels
    for (int i = 0; i < 256 * 224 / 8 - 1; i++) {
        const int y = i * 8 / 256;
        const int base_x = (i * 8) % 256;
        int cur_byte = m->memory[start_addr + i];

        for (int b = 0; b < 8; b++) {
            if ((cur_byte >> b) & 1) {
                int px = base_x + b;
                int py = y;

                // space invaders' screen is rotated 90 degrees anti-clockwise
                // so we invert the coordinates:
                const int temp_x = px;
                px = py;
                py = -temp_x + 256;

                glBegin(GL_QUADS);
                    // color management
                    if (py > 32 && py < 64) { // red
                        glColor3f(1.f, 0.f, 0.f);
                    }
                    else if (py > 184 && py < 184 + 56) { // green
                        glColor3f(0.f, 1.f, 0.f);
                    }
                    else if (py > 240) {
                        if (px < 16 || px > 134) { // white
                            glColor3f(1.f, 1.f, 1.f);
                        }
                        else { // green
                            glColor3f(0.f, 1.f, 0.f);
                        }
                    }
                    else {
                        glColor3f(1.f, 1.f, 1.f);
                    }

                    glVertex2f(px, py);
                    glVertex2f(px + 1, py);
                    glVertex2f(px + 1, py + 1);
                    glVertex2f(px, py + 1);
                glEnd();
            }
        }
    }
}
