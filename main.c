#include "include/GLFW/glfw3.h"
#include "include/logic.h"
#include <stdio.h>

#define WIDTH 700
#define HEIGHT 700

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    int side = (width < height) ? width-50 : height-50;
    int offsetX = (width - side) / 2;
    int offsetY = (height - side) / 2;
    glViewport(offsetX, offsetY, side, side);
}

void drawGrid(int gridSize) {    
    float margin = 0.05f;
    float gridWidth = WIDTH * (1 - 2*margin);
    float gridHeight = HEIGHT * (1 - 2*margin);
    
    float startX = WIDTH * margin;
    float startY = HEIGHT * margin;
    
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    
    float cellSize = gridWidth / gridSize;
    
    glBegin(GL_LINES);
    
    for (int i = 0; i <= gridSize; i++) {
        float x = startX + i * cellSize;
        glVertex2f(x, startY);
        glVertex2f(x, startY + gridHeight);
    }
    
    for (int j = 0; j <= gridSize; j++) {
        float y = startY + j * cellSize;
        glVertex2f(startX, y);
        glVertex2f(startX + gridWidth, y);
    }
    
    glEnd();
}

void drawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);

    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);

    glEnd();
}

const int gridSize = 30; // 30x30

int main(void){

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(WIDTH,HEIGHT, "Game of Life", NULL, NULL);

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

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    while (!glfwWindowShouldClose(window))
    {
        
        glfwGetFramebufferSize(window, &width, &height);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT); 
        
        drawGrid(gridSize);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}