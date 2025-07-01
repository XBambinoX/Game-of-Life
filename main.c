#include "include/GLFW/glfw3.h"
#include <stdio.h>


void error_callback(int error, const char* description){
    fprintf(stderr, "Error: %s\n", description);
    }

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    int side = (width < height) ? width-50 : height-50;
    int offsetX = (width - side) / 2;
    int offsetY = (height - side) / 2;
    glViewport(offsetX, offsetY, side, side);
}

const int gridSize = 40;

void drawGrid(int N) {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);

    float margin = 0.999f;
    float start = -margin;
    float end = margin;
    float step = (2.0f * margin) / N;      

    for (int i = 0; i <= N; ++i) {
        float x = start + i * step;
        glVertex2f(x, start);
        glVertex2f(x, end);
    }

    for (int j = 0; j <= N; ++j) {
        float y = start + j * step;
        glVertex2f(start, y);
        glVertex2f(end, y);
    }

    glEnd();
}
int main(void){

    GLFWwindow* window;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        return -1;

    window = glfwCreateWindow(700, 600, "Game of Life", NULL, NULL);

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