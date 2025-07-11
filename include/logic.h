#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool** CreateField(const unsigned short size) {
    bool** field = (bool**)malloc(size * sizeof(bool*));
    for (unsigned short i = 0; i < size; i++) {
        field[i] = (bool*)malloc(size * sizeof(bool));
        for (unsigned short j = 0; j < size; j++) {
            field[i][j] = 0;
        }
    }
    return field;
}

static void NextStep(bool** current, bool** next, const unsigned short size ) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (current[i][j] == 0) {
                int all_dead = 1;
                for (int di = -1; di <= 1 && all_dead; di++) {
                    for (int dj = -1; dj <= 1 && all_dead; dj++) {
                        if (di == 0 && dj == 0) continue;
                        int ni = (i + di + size) % size;
                        int nj = (j + dj + size) % size;
                        if (current[ni][nj] != 0) {
                            all_dead = 0;
                            break;
                        }
                    }
                }
                if (all_dead) {
                    next[i][j] = 0;
                    continue;
                }
            }
            int neighbors = 0;
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    if (di == 0 && dj == 0) continue;
                    int ni = (i + di + size) % size;
                    int nj = (j + dj + size) % size;
                    neighbors += current[ni][nj];
                }
            }

            if (current[i][j]) {
                next[i][j] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                next[i][j] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
}

static void FreeField(bool** field, const unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        free(field[i]);
    }
    free(field);
}