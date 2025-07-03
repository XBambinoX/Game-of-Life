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

static void NextStep(bool** current, bool** next, const unsigned short size) {
    for (unsigned short y = 0; y < size; y++) {
        for (unsigned short x = 0; x < size; x++) {
            unsigned short alive_neighbors = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < size && ny >= 0 && ny < size) {
                        if (current[ny][nx] == 1) {
                            alive_neighbors++;
                        }
                    }
                }
            }
            if (current[y][x] == 1) {
                next[y][x] = (alive_neighbors == 2 || alive_neighbors == 3) ? 1 : 0;
            }
            else {
                next[y][x] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }
}

void FreeField(bool** field, const unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        free(field[i]);
    }
    free(field);
}