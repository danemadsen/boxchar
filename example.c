#include "boxchar.h"

int main() {
    bc_init();

    int width, height, new_width, new_height;
    bc_termsize(&width, &height);

    float x_ratio = 0.5f;
    float y_ratio = 0.5f;

    int x = (int)(width * x_ratio);
    int y = (int)(height * y_ratio);

    bc_clear();

    while (1) {
        // Get updated terminal size
        bc_termsize(&new_width, &new_height);

        // If the terminal size has changed, update x and y based on the relative ratios
        if (new_width != width || new_height != height) {
            x = (int)(new_width * x_ratio);
            y = (int)(new_height * y_ratio);
            
            width = new_width;
            height = new_height;
        }

        bc_startcolor((bc_colorpair) {BC_RED, BC_GREEN});
        bc_putchar((bc_point) {x, y}, L'☻');
        bc_endcolor();

        int ch = bc_getchar();
        bc_clear();

        if (ch == 'w' && y > 0) {
            y--; y_ratio = (float)y / height;
        } else if (ch == 's' && y < height - 1) {
            y++; y_ratio = (float)y / height;
        } else if (ch == 'a' && x > 0) {
            x--; x_ratio = (float)x / width;
        } else if (ch == 'd' && x < width - 1) {
            x++; x_ratio = (float)x / width;
        } else if (ch == 'q') {
            break;  // Exit the loop if 'q' is pressed
        }

        #ifdef _WIN32
            Sleep(10); // Sleep for 10 milliseconds on Windows
        #else
            usleep(10000); // Sleep for 10 milliseconds (50000 microseconds) on UNIX-like systems
        #endif
    }

    bc_end();
    return 0;
}
