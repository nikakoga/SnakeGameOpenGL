#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GRID_COUNT_X 8
#define GRID_COUNT_Y 8

GLfloat verticles[] = {
    0.0, 0.0,
    0.0, 1.0,
    1.0, 1.0,
    1.0, 0.0
};

GLuint indices[] = {
    0, 1, 2,
    2, 3, 0
};

const GLchar* vShader_Code =
"#version 330 core\n"
"layout (location = 0) in vec2 v_pos;\n"
"uniform float POSITION;\n"
"uniform vec2 GRID_COUNT;\n"
"void main() {\n"
"   vec2 GRID_SIZE = vec2(2.0 / GRID_COUNT.x, 2.0 / GRID_COUNT.y);\n"
"   vec2 pos_center = vec2((mod(POSITION, GRID_COUNT.x) * GRID_SIZE.x) - 1.0 + (GRID_SIZE.x) / 2.0,\n"
"                          1.0 - GRID_SIZE.y - (floor(POSITION / GRID_COUNT.x) * GRID_SIZE.y) + (GRID_SIZE.y / 2.0));\n"
"   gl_Position = vec4(\n"
"       pos_center.x - (GRID_SIZE.x / 2.0) + (v_pos.x * GRID_SIZE.x),\n"
"       pos_center.y - (GRID_SIZE.y / 2.0) + (v_pos.y * GRID_SIZE.y),\n"
"       0.0, 1.0\n"
"   );\n"
"}\0";

const GLchar* fShader_Code =
"#version 330 core\n"
"out vec4 fragment_color;\n"
"void main() {\n"
"   fragment_color = vec4(1.0, 0.5, 0.5, 1.0);\n"
"}\0";

struct Button
{
    GLboolean down, last, pressed;
};
struct Keyboard
{
    struct Button keys[GLFW_KEY_LAST];
};
struct Keyboard keyboard = {};

enum Directions
{
    LEFT = 1,
    RIGHT = 2,
    UP = 4,
    DOWN = 8,
    PAUSE = 16
};

struct Snake
{
    GLuint counter;
    GLuint positions[GRID_COUNT_X * GRID_COUNT_Y];
    GLuint direction;
};

struct Snake snake = {};
GLuint foodPosition = 0;

GLuint generateRandomPosition(void)
{
    GLuint randomPosition = (GLuint)(rand() % (GRID_COUNT_X * GRID_COUNT_Y));
    GLboolean free_position;
    do {
        free_position = GL_TRUE;
        for (GLuint i = 0; i < snake.counter; i++)
        {
            if (snake.positions[i] == randomPosition)
            {
                free_position = GL_FALSE;
                randomPosition++;
                if (randomPosition >= (GRID_COUNT_X * GRID_COUNT_Y))
                {
                    randomPosition %= (GRID_COUNT_X * GRID_COUNT_Y);
                }
            }
        }
    } while (!free_position);
    return randomPosition;
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key < 0)
    {
        return;
    }

    switch (action)
    {
    case GLFW_PRESS:
        keyboard.keys[key].down = GL_TRUE;
        break;
    case GLFW_RELEASE:
        keyboard.keys[key].down = GL_FALSE;
        break;
    default:
        break;
    }
}

void _button_array_update(struct Button* buttons)
{
    for (int i = 0; i < GLFW_KEY_LAST; i++)
    {
        buttons[i].pressed = buttons[i].down && !buttons[i].last;
        buttons[i].last = buttons[i].down;
    }
}

void keyFunctions(GLFWwindow* window)
{
    _button_array_update(keyboard.keys);

    if (keyboard.keys[GLFW_KEY_ESCAPE].down)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

//void initOpenGLProgram(GLFWwindow* window) {
//    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� raz, na pocz�tku programu************
//    glClearColor(0, 1, 0, 1);
//    glEnable(GL_DEPTH_TEST);
//    keyFunctions(window);
//
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Wyczy�� bufor koloru i g��boko�ci
//    //glfwSetWindowSizeCallback(window, windowResizeCallback);
//    //glfwSetKeyCallback(window, keyCallback);
//}

void freeOpenGLProgram(GLFWwindow* window) {
    //************Tutaj umieszczaj kod, kt�ry nale�y wykona� po zako�czeniu p�tli g��wnej************
}

int main(int argc, char const* argv[]) {

    srand(time(NULL));
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW!\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "SNAKE", NULL, NULL);

    if (!window) { // Je�eli okna nie uda�o si� utworzy�, to zamknij program
        fprintf(stderr, "Nie mo�na utworzy� okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window); // Od tego momentu kontekst okna staje si� aktywny i polecenia OpenGL b�d� dotyczy� w�a�nie jego.
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1); // Czekaj na 1 powr�t plamki przed pokazaniem ukrytego bufora

    // Inicjalizacja GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD!\n");
        return -1;
    }

    GLuint vShader, fShader, shader_programm;

    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vShader_Code, NULL);
    glCompileShader(vShader);

    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fShader_Code, NULL);
    glCompileShader(fShader);

    shader_programm = glCreateProgram();
    glAttachShader(shader_programm, vShader);
    glAttachShader(shader_programm, fShader);

    glLinkProgram(shader_programm);
    glDeleteShader(vShader);
    glDeleteShader(fShader);

    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticles), verticles, GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);

    snake.counter = 1;
    snake.positions[0] = (GLuint)(rand() % (GRID_COUNT_X * GRID_COUNT_Y));
    snake.direction = PAUSE;

    foodPosition = generateRandomPosition();

    while (!glfwWindowShouldClose(window)) { // Tak d�ugo jak okno nie powinno zosta� zamkni�te

        //initOpenGLProgram(window); // Operacje inicjuj�ce
        glClearColor(0, 1, 0, 1);
        glEnable(GL_DEPTH_TEST);
        keyFunctions(window);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Wyczy�� bufor koloru i g��boko�ci

        glBindVertexArray(VAO);
        glUseProgram(shader_programm);
        glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), snake.positions[0]);
        glUniform2f(glGetUniformLocation(shader_programm, "GRID_COUNT"), (GLfloat)GRID_COUNT_X, (GLfloat)GRID_COUNT_Y);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents(); // Wykonaj procedury callback w zale�no�ci od zdarze� jakie zasz�y
        glfwSwapBuffers(window); // Zamie� bufory
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window); // Usu� kontekst OpenGL i okno
    glfwTerminate(); // Zwolnij zasoby zaj�te przez GLFW
    exit(EXIT_SUCCESS);
}
