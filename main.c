#include "include/GLFW/glfw3.h"
#include "include/logic.h"
#include "include/json.h"

#define SIZE 700
#define MAX_KEYS 1024

bool** CreateField(const unsigned short size);
void NextStep(bool** current, bool** next, const unsigned short size);
char* ReadJson(const char* filename);
returnPair ParseJson(char* data);
void FreeField(bool** field, unsigned short size);

static void error_callback(int error, const char* description) {
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
unsigned short fieldSize = 40; // Default field size
float stepDelay = 0.05f; // Default step delay in seconds

bool **field = NULL;
bool **newField = NULL;
bool **savedField = NULL;

const float margin = 0.05f;
const float gridSize = SIZE * (1 - 2 * margin);
const float startXY = (SIZE - gridSize) / 2;
float cellSize;

double lastStepTime = 0.0;
bool shouldWait = true;

short lastCellI = -1;
short lastCellJ = -1;

bool keyStates[MAX_KEYS] = {false};

static void drawRectangle(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);
    glColor3f(0.7f, 0.7f, 0.7f);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glVertex2f(x4, y4);
    glEnd();
}

static void drawGrid() {
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    
    glBegin(GL_LINES);
    
    for (unsigned short i = 0; i <= fieldSize; i++) {
        float x = startXY + i * cellSize;
        float y = x;
        glVertex2f(x, startXY);
        glVertex2f(x, startXY + gridSize);
        glVertex2f(startXY, y);
        glVertex2f(startXY + gridSize, y);
    }
    glEnd();
}

static void fillField(bool** field, unsigned short size) {
    for(unsigned short i = 0; i < size; i++) {
        for (unsigned short j = 0; j < size; j++) {
            if (field[i][j]) {
                float x = startXY + j * cellSize;
                float y = startXY + (size - 1 - i) * cellSize;
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

static void writeFieldToFile(bool** field, unsigned short size, const char* filename) {
    FILE *file = fopen(filename, "wb");
    for (unsigned short row = 0; row < size; row++) {
        for (unsigned short col = 0; col < size; col++) {
            if (field[row][col]) {
                unsigned short coords[2] = {row, col};
                fwrite(coords, sizeof(unsigned short), 2, file);
            }
        }
    }
    fclose(file);
}

static void readFieldFromFile(bool** field, unsigned short size, const char* filename) {
    FILE *file = fopen(filename, "rb");
    unsigned short coords[2];
    while (fread(coords, sizeof(unsigned short), 2, file) == 2) {
        field[coords[0]][coords[1]] = true;
    }
    fclose(file);
}

typedef struct {
    int key1;
    int key2;
    const char* filename;
} keyCombo;

keyCombo combosW[] = {                          // Combos for writing files
    {GLFW_KEY_W, GLFW_KEY_0, "file_w.bin"},
    {GLFW_KEY_W, GLFW_KEY_1, "file_w1.bin"},
    {GLFW_KEY_W, GLFW_KEY_2, "file_w2.bin"},
    {GLFW_KEY_W, GLFW_KEY_3, "file_w3.bin"},
    {GLFW_KEY_W, GLFW_KEY_4, "file_w4.bin"},
    {GLFW_KEY_W, GLFW_KEY_5, "file_w5.bin"},
    {GLFW_KEY_W, GLFW_KEY_6, "file_w6.bin"},
    {GLFW_KEY_W, GLFW_KEY_7, "file_w7.bin"},
    {GLFW_KEY_W, GLFW_KEY_8, "file_w8.bin"},
    {GLFW_KEY_W, GLFW_KEY_9, "file_w9.bin"},
};

keyCombo combosR[] = {                          // Combos for reading files
    {GLFW_KEY_R, GLFW_KEY_0, "file_w.bin"},
    {GLFW_KEY_R, GLFW_KEY_1, "file_w1.bin"},
    {GLFW_KEY_R, GLFW_KEY_2, "file_w2.bin"},
    {GLFW_KEY_R, GLFW_KEY_3, "file_w3.bin"},
    {GLFW_KEY_R, GLFW_KEY_4, "file_w4.bin"},
    {GLFW_KEY_R, GLFW_KEY_5, "file_w5.bin"},
    {GLFW_KEY_R, GLFW_KEY_6, "file_w6.bin"},
    {GLFW_KEY_R, GLFW_KEY_7, "file_w7.bin"},
    {GLFW_KEY_R, GLFW_KEY_8, "file_w8.bin"},
    {GLFW_KEY_R, GLFW_KEY_9, "file_w9.bin"},
};

keyCombo combosD[] = {                          // Combos for deleting files
    {GLFW_KEY_D, GLFW_KEY_0, "file_w.bin"},
    {GLFW_KEY_D, GLFW_KEY_1, "file_w1.bin"},
    {GLFW_KEY_D, GLFW_KEY_2, "file_w2.bin"},
    {GLFW_KEY_D, GLFW_KEY_3, "file_w3.bin"},
    {GLFW_KEY_D, GLFW_KEY_4, "file_w4.bin"},
    {GLFW_KEY_D, GLFW_KEY_5, "file_w5.bin"},
    {GLFW_KEY_D, GLFW_KEY_6, "file_w6.bin"},
    {GLFW_KEY_D, GLFW_KEY_7, "file_w7.bin"},
    {GLFW_KEY_D, GLFW_KEY_8, "file_w8.bin"},
    {GLFW_KEY_D, GLFW_KEY_9, "file_w9.bin"},
};

#define NUM_COMBOS (sizeof(combosW) / sizeof(combosW[0]))
bool comboTriggered[NUM_COMBOS] = {false};

static void KeyPressed(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        shouldWait = !shouldWait;
    }
    
    if (key == GLFW_KEY_C && action == GLFW_PRESS){ // For clearing the field
        shouldWait = true;
        for (unsigned short i = 0; i < fieldSize; i++) {
            for (unsigned short j = 0; j < fieldSize; j++) {
                field[i][j] = 0;
            }
        }
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS){ // For saving the field
        shouldWait = true;
        if (savedField != NULL) {
            FreeField(savedField, fieldSize);
        }
        savedField = CreateField(fieldSize);
        for (unsigned short i = 0; i < fieldSize; i++) {
            for (unsigned short j = 0; j < fieldSize; j++) {
                savedField[i][j] = field[i][j];
            }
        }
    }

    if (key == GLFW_KEY_P && action == GLFW_PRESS){ // For loading the saved field
        if (savedField != NULL) {
            shouldWait = true;
            FreeField(field, fieldSize);
            field = CreateField(fieldSize);
            for (unsigned short i = 0; i < fieldSize; i++) {
                for (unsigned short j = 0; j < fieldSize; j++) {
                    field[i][j] = savedField[i][j];
                }
            }
        }
    }

    if (key >= 0 && key < MAX_KEYS) { // Update key states
        if (action == GLFW_PRESS) {
            keyStates[key] = true;
        }
        else if (action == GLFW_RELEASE) {
            keyStates[key] = false;
        }
    }  

    for (int i = 0; i < NUM_COMBOS; i++) { // Check for write combos
        bool bothPressed = keyStates[combosW[i].key1] && keyStates[combosW[i].key2];
        if (bothPressed && !comboTriggered[i]) {
            writeFieldToFile(field, fieldSize, combosW[i].filename);
            comboTriggered[i] = true;
        }
        else if (!bothPressed) {
            comboTriggered[i] = false;
        }
    }

    for (int i = 0; i < NUM_COMBOS; i++) { // Check for read combos
        bool bothPressed = keyStates[combosR[i].key1] && keyStates[combosR[i].key2];
        if (bothPressed && !comboTriggered[i]) {
            shouldWait = true;
            FreeField(field, fieldSize);
            field = CreateField(fieldSize);
            readFieldFromFile(field, fieldSize, combosR[i].filename);
            comboTriggered[i] = true;
        }
        else if (!bothPressed) {
            comboTriggered[i] = false;
        }
    }

    for (int i = 0; i < NUM_COMBOS; i++) { // Check for delete combos
        bool bothPressed = keyStates[combosD[i].key1] && keyStates[combosD[i].key2];
        if (bothPressed && !comboTriggered[i]) {
            remove(combosD[i].filename);
        }
        else if (!bothPressed) {
            comboTriggered[i] = false;
        }
    }
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (xpos > startXY && xpos < SIZE - startXY && ypos > startXY && ypos < SIZE - startXY) {
            unsigned short i = (ypos - startXY) / cellSize;
            unsigned short j = (xpos - startXY) / cellSize;
            if (lastCellI != i || lastCellJ != j) {
                field[i][j] = !field[i][j]; //change cell status
                lastCellI = i;
                lastCellJ = j;
            }
        }
    }
}

int main(void) {
    cellSize = gridSize / fieldSize;

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
    window = glfwCreateWindow(SIZE, SIZE, "Game of Life", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    setWindowIcon(window, "src/icon.png");
    glfwMakeContextCurrent(window);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, SIZE, 0, SIZE, -1, 1);
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

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        drawGrid();
        double currentTime = glfwGetTime();

        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            lastCellI = -1;
            lastCellJ = -1;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            mouseButtonCallback(window, GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS, 0);
        }

        fillField(field, fieldSize);
        
        if (!shouldWait && currentTime - lastStepTime >= stepDelay) {
            NextStep(field, newField,fieldSize);
            swapFields(&field, &newField, fieldSize);
            lastStepTime = currentTime;
        }

        glfwSetKeyCallback(window, KeyPressed);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    FreeField(field, fieldSize);
    FreeField(newField, fieldSize);
    FreeField(savedField, fieldSize);

    glfwTerminate();
    return 0;
}