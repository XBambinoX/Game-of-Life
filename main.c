#include "include/GLFW/glfw3.h"
#include "include/logic.h"

#define WIDTH 700
#define HEIGHT 700

static bool** CreateField(const unsigned short width, const unsigned short height);
static void NextStep(bool** current, bool** next, unsigned short width, unsigned short height);
void FreeField(bool** field, unsigned short size);


void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    unsigned short side = (width < height) ? width-50 : height-50;
    unsigned short offsetX = (width - side) / 2;
    unsigned short offsetY = (height - side) / 2;
    glViewport(offsetX, offsetY, side, side);
}

static void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    GLFWimage image;
    image.pixels = stbi_load(iconPath, &image.width, &image.height, 0, 4);
    if (image.pixels) {
        glfwSetWindowIcon(window, 1, &image);
        stbi_image_free(image.pixels);
    }
}

const unsigned short gridSize = 30; // 30x30
const float margin = 0.01f;
const float startX = WIDTH * margin;
const float startY = HEIGHT * margin;
const float gridWidth = WIDTH * (1 - 2*margin);
const float gridHeight = HEIGHT * (1 - 2*margin);
const float cellSize = gridWidth / gridSize;

void drawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);

    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);

    glEnd();
}

void drawGrid(unsigned short gridSize) {    

    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    
    float cellSize = gridWidth / gridSize;
    
    glBegin(GL_LINES);
    
    for (unsigned short i = 0; i <= gridSize; i++) {
        float x = startX + i * cellSize;
        glVertex2f(x, startY);
        glVertex2f(x, startY + gridHeight);
    }
    
    for (unsigned short j = 0; j <= gridSize; j++) {
        float y = startY + j * cellSize;
        glVertex2f(startX, y);
        glVertex2f(startX + gridWidth, y);
    }

    glEnd();
}

static void fillField(bool** field, unsigned short size) {
    for(unsigned short i = 0; i < size; i++) {
        for (unsigned short j = 0; j < size; j++) {
            if (field[i][j] == 1) {
                unsigned short x = j, y = size - 1 - i;
                drawRectangle(startX + x * cellSize, startY + y * cellSize, 
                              startX + (x + 1) * cellSize, startY + y * cellSize, 
                              startX + (x + 1) * cellSize, startY + (y + 1) * cellSize, 
                              startX + x * cellSize, startY + (y + 1) * cellSize);
            }
        }
    }
}

static void swapFields(bool ***current, bool ***next, unsigned short size) {
    bool **temp = *current;
    *current = *next;
    *next = temp;
}

double lastStepTime = 0.0;
float stepDelay = 0.05;
bool shouldWait = true;

static void key_space(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
       shouldWait = !shouldWait;
    }
    else false;
}



int main(void){

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(WIDTH,HEIGHT, "Game of Life", NULL, NULL);

    setWindowIcon(window, "icon.png");

    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    framebuffer_size_callback(window, width, height);

    glfwSetWindowSizeLimits(window, 550, 450, 850, 750);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    bool **field = CreateField(gridSize, gridSize);
    bool **newField = CreateField(gridSize, gridSize);
    field[4][4] = 1;
    field[4][5] = 1;
    field[3][6] = 1;
    field[4][5] = 1;
    field[2][5] = 1;
    field[3][5] = 1;
    field[7][7] = 1;
    field[16][20] = 1;
    field[16][21] = 1;
    field[15][20] = 1;
    field[14][19] = 1;
    field[15][19] = 1;
    field[14][14] = 1;
    field[14][15] = 1;
    field[14][17] = 1;

    while (!glfwWindowShouldClose(window))
    {
        
        glfwGetFramebufferSize(window, &width, &height);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawGrid(gridSize);

        double currentTime = glfwGetTime();

        glfwSetKeyCallback(window, key_space);
        
        fillField(field, gridSize);
        
        if (!shouldWait && currentTime - lastStepTime >= stepDelay) {
            NextStep(field, newField, gridSize, gridSize);
            swapFields(&field, &newField, gridSize);
            lastStepTime = currentTime;
        }

        fillField(newField, gridSize);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    FreeField(field, gridSize);
    FreeField(newField, gridSize);

    glfwTerminate();
    return 0;
}