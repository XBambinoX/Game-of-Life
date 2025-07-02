#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

static bool** CreateField(const unsigned short width, const unsigned short height) {
    bool** field = (bool**)malloc(height * sizeof(bool*));
    for (unsigned short i = 0; i < height; i++) {
        field[i] = (bool*)malloc(width * sizeof(bool));
        for (unsigned short j = 0; j < width; j++) {
            field[i][j] = 0;
        }
    }
    return field;
}

static void NextStep(bool** current, bool** next, unsigned short width, unsigned short height) {
    for (unsigned short y = 0; y < height; y++) {
        for (unsigned short x = 0; x < width; x++) {
            unsigned short alive_neighbors = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dx == 0 && dy == 0) continue;
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
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

void FreeField(bool** field, unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        free(field[i]);
    }
    free(field);
}