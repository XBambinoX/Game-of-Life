#include "include/GLFW/glfw3.h"
#include "include/logic.h"

#define WIDTH 700
#define HEIGHT 700
#define FIELD_SIZE 30 

static bool** CreateField(const unsigned short size);
static void NextStep(bool** current, bool** next, const unsigned short size);
void FreeField(bool** field, unsigned short size);

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    float aspect = (float)WIDTH / HEIGHT;
    int viewportWidth = width;
    int viewportHeight = (int)(width / aspect);
    
    if (viewportHeight > height) {
        viewportHeight = height;
        viewportWidth = (int)(height * aspect);
    }
    
    int x = (width - viewportWidth) / 2;
    int y = (height - viewportHeight) / 2;
    glViewport(x, y, viewportWidth, viewportHeight);
}

static void setWindowIcon(GLFWwindow* window, const char* iconPath) {
    GLFWimage image;
    image.pixels = stbi_load(iconPath, &image.width, &image.height, 0, 4);
    if (image.pixels) {
        glfwSetWindowIcon(window, 1, &image);
        stbi_image_free(image.pixels);
    }
}

bool **field = NULL;
bool **newField = NULL;

float startX, startY;
float gridWidth, gridHeight;
float cellSize;

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
    cellSize = gridWidth / FIELD_SIZE;
}

void drawGrid() {    
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    
    glBegin(GL_LINES);
    
    for (unsigned short i = 0; i <= FIELD_SIZE; i++) {
        float x = startX + i * cellSize;
        glVertex2f(x, startY);
        glVertex2f(x, startY + gridHeight);
    }
    
    for (unsigned short j = 0; j <= FIELD_SIZE; j++) {
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

double lastStepTime = 0.0;
float stepDelay = 0.05;
bool shouldWait = true;

static void key_space(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        shouldWait = !shouldWait;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        calculateGridDimensions(windowWidth, windowHeight);
        
        if(xpos > startX && xpos < WIDTH - startX && ypos > startY && ypos < HEIGHT - startY) {
            unsigned short i = (ypos - startY) / cellSize;
            unsigned short j = (xpos - startX) / cellSize;
            field[i][j] = !field[i][j]; //change cell status
        }
    }
}

int main(void) {

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Game of Life", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    setWindowIcon(window, "src/icon.png");
    glfwMakeContextCurrent(window);

    calculateGridDimensions(WIDTH, HEIGHT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, WIDTH, 0, HEIGHT, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    field = CreateField(FIELD_SIZE);
    newField = CreateField(FIELD_SIZE);

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

        glfwSetKeyCallback(window, key_space);
        fillField(field, FIELD_SIZE);
        
        if (!shouldWait && currentTime - lastStepTime >= stepDelay) {
            NextStep(field, newField,FIELD_SIZE);
            swapFields(&field, &newField, FIELD_SIZE);
            lastStepTime = currentTime;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    FreeField(field, FIELD_SIZE);
    FreeField(newField, FIELD_SIZE);

    glfwTerminate();
    return 0;
}