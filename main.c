#include "include/GLFW/glfw3.h"
#include "include/logic.h"
#include "include/json.h"
#include <stdlib.h>

#define WIDTH 700
#define HEIGHT 700

static bool** CreateField(const unsigned short size);
static void NextStep(bool** current, bool** next, const unsigned short size);
static char* ReadJson(const char* filename);
static returnPair ParseJson(char* data);
static void FreeField(bool** field, unsigned short size);

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    GLFWimage image;
    image.pixels = stbi_load(iconPath, &image.width, &image.height, 0, 4);
    if (image.pixels) {
        glfwSetWindowIcon(window, 1, &image);
        stbi_image_free(image.pixels);
    }
}

//global variables declaration
int fieldSize = 30;
float stepDelay = 0.05;

bool **field = NULL;
bool **newField = NULL;
bool **savedField = NULL;

float startX, startY;
float gridWidth, gridHeight;
float cellSize;

double lastStepTime = 0.0;
bool shouldWait = true;

short lastCellI = -1;
short lastCellJ = -1;

void drawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

void calculateGridDimensions(int windowWidth, int windowHeight) {
    float margin = 0.05f; 
    float availableWidth = windowWidth * (1 - 2 * margin);
    float availableHeight = windowHeight * (1 - 2 * margin);
    float gridSide = (availableWidth < availableHeight) ? availableWidth : availableHeight;
    
    startX = (windowWidth - gridSide) / 2;
    startY = (windowHeight - gridSide) / 2;
    gridWidth = gridSide;
    gridHeight = gridSide;
    cellSize = gridWidth / fieldSize;
}

void drawGrid() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    
    glBegin(GL_LINES);
    
    for (unsigned short i = 0; i <= fieldSize; i++) {
        float x = startX + i * cellSize;
        glVertex2f(x, startY);
        glVertex2f(x, startY + gridHeight);
    }
    
    for (unsigned short j = 0; j <= fieldSize; j++) {
        float y = startY + j * cellSize;
        glVertex2f(startX, y);
        glVertex2f(startX + gridWidth, y);
    }

    glEnd();
}

static void fillField(bool** field, unsigned short size) {
    for(unsigned short i = 0; i < size; i++) {
        for (unsigned short j = 0; j < size; j++) {
            if (field[i][j]) {
                float x = startX + j * cellSize;
                float y = startY + (size - 1 - i) * cellSize;
                drawRectangle(x, y, 
                            x + cellSize, y, 
                            x + cellSize, y + cellSize, 
                            x, y + cellSize);
            }
        }
    }
}

static void swapFields(bool ***current, bool ***next, unsigned short size) {
    bool **temp = *current;
    *current = *next;
    *next = temp;
}

static void key_pressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        shouldWait = !shouldWait;
    }
    if(key == GLFW_KEY_C && action == GLFW_PRESS){ // For clearing the field
        shouldWait = true;
        for(int i = 0; i < fieldSize; i++) {
            for(int j = 0; j < fieldSize; j++) {
                field[i][j] = 0;
            }
        }
    }
    if(key == GLFW_KEY_S && action == GLFW_PRESS){ // For saving the field
        shouldWait = true;
        if(savedField != NULL) {
            FreeField(savedField, fieldSize);
        }
        savedField = CreateField(fieldSize);
        for(int i = 0; i < fieldSize; i++) {
            for(int j = 0; j < fieldSize; j++) {
                savedField[i][j] = field[i][j];
            }
        }
    }
    if(key == GLFW_KEY_P && action == GLFW_PRESS){ // For loading the saved field
        if(savedField != NULL) {
            shouldWait = true;
            FreeField(field, fieldSize);
            field = CreateField(fieldSize);
            for(int i = 0; i < fieldSize; i++) {
                for(int j = 0; j < fieldSize; j++) {
                    field[i][j] = savedField[i][j];
                }
            }
        }
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        calculateGridDimensions(WIDTH, HEIGHT);
        
        if(xpos > startX && xpos < WIDTH - startX && ypos > startY && ypos < HEIGHT - startY) {
            unsigned short i = (ypos - startY) / cellSize;
            unsigned short j = (xpos - startX) / cellSize;
            if(lastCellI != i || lastCellJ != j) {
                field[i][j] = !field[i][j]; //change cell status
                lastCellI = i;
                lastCellJ = j;
            }
        }
    }
}

int main(void) {
    char* rawJson = ReadJson("configs/setup.json");
    if (rawJson != NULL){
        returnPair Pair = ParseJson(rawJson);
        for (int i = 0; i < Pair.size; i++){
            if (strcmp(Pair.data[i].key,"fieldSize") == 0) {
                fieldSize = atoi(Pair.data[i].value);
            }
            else if (strcmp(Pair.data[i].key,"stepDelay") == 0) {
                stepDelay = (float)atof(Pair.data[i].value);
            }
        }
        free(Pair.data);
    }
    free(rawJson);
    
    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Game of Life", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    setWindowIcon(window, "src/icon.png");
    glfwMakeContextCurrent(window);

    calculateGridDimensions(WIDTH, HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    field = CreateField(fieldSize);
    newField = CreateField(fieldSize);

    // Initial pattern (Glider)
    field[1][2] = 1;
    field[2][3] = 1;
    field[3][1] = 1;
    field[3][2] = 1;
    field[3][3] = 1;

    while (!glfwWindowShouldClose(window)) {
        calculateGridDimensions(WIDTH, HEIGHT);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawGrid();
        double currentTime = glfwGetTime();

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            lastCellI = -1;
            lastCellJ = -1;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mouse_button_callback(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        }

        fillField(field, fieldSize);
        
        if (!shouldWait && currentTime - lastStepTime >= stepDelay) {
            NextStep(field, newField,fieldSize);
            swapFields(&field, &newField, fieldSize);
            lastStepTime = currentTime;
        }

        glfwSetKeyCallback(window, key_pressed);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    FreeField(field, fieldSize);
    FreeField(newField, fieldSize);
    FreeField(savedField, fieldSize);

    glfwTerminate();
    return 0;
}