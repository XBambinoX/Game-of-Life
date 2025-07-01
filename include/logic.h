#include <stdlib.h>

static int** CreateField(const int width, const int height) {
    int** field = (int**)malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++) {
        field[i] = (int*)malloc(width * sizeof(int));
        for (int j = 0; j < width; j++) {
            field[i][j] = 0;
        }
    }
    return field;
}

static void NextStep(int** current, int** next, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int alive_neighbors = 0;
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
            } else {
                next[y][x] = (alive_neighbors == 3) ? 1 : 0;
            }
        }
    }
}