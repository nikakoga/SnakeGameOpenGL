#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ShaderProgram.h"
#include "Snake.h"


#define GRID_COUNT_X 17
#define GRID_COUNT_Y 17
#define SPEED 6

GLfloat color_tail[] = { 0.6, 0.88, 0.6 };
GLfloat color_head[] = { 0.5, 1.0, 0.5 };
GLfloat color_food[] = { 1.0, 0.5, 0.5 };
GLfloat color_background[] = { 0.14, 0.16, 0.18 };

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

struct Button {
    GLboolean down, last, pressed;
};
struct Keyboard {
    struct Button keys[GLFW_KEY_LAST];
};
struct Keyboard keyboard = {};

Snake snake(GRID_COUNT_X, GRID_COUNT_Y);

GLdouble TIME_NOW, TIME_LAST, TIME_DELTA, TIME_SUM;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key < 0) {
        return;
    }

    switch (action) {
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

void _button_array_update(struct Button* buttons) {
    for (int i = 0; i < GLFW_KEY_LAST; i++) {
        buttons[i].pressed = buttons[i].down && !buttons[i].last;
        buttons[i].last = buttons[i].down;
    }
}

void keyFunctions(GLFWwindow* window) {
    _button_array_update(keyboard.keys);
    GLuint _last_snake_direction = snake.getDirection();

    if (keyboard.keys[GLFW_KEY_ESCAPE].down)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (keyboard.keys[GLFW_KEY_SPACE].pressed)
        snake.setDirection(snake.getDirection() | PAUSE);

    if (keyboard.keys[GLFW_KEY_LEFT].pressed)
        if ((snake.getDirection() != LEFT) && !(snake.getDirection() & RIGHT))
            snake.setDirection(LEFT);

    if (keyboard.keys[GLFW_KEY_RIGHT].pressed)
        if (!(snake.getDirection() & LEFT) && (snake.getDirection() != RIGHT))
            snake.setDirection(RIGHT);

    if (keyboard.keys[GLFW_KEY_UP].pressed)
        if ((snake.getDirection() != UP) && !(snake.getDirection() & DOWN))
            snake.setDirection(UP);

    if (keyboard.keys[GLFW_KEY_DOWN].pressed)
        if (!(snake.getDirection() & UP) && (snake.getDirection() != DOWN))
            snake.setDirection(DOWN);

    if (snake.getDirection() != _last_snake_direction)
    {
        snake.update();
        TIME_SUM = 0.0;
    }
}

void freeOpenGLProgram(GLFWwindow* window) {
    // Cleanup code
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath);

int main(int argc, char const* argv[]) {
    if (!glfwInit()) {
        fprintf(stderr, "ERROR: Failed to initialize GLFW!\n");
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window = glfwCreateWindow(500, 500, "SNAKE", NULL, NULL);

    if (!window) {
        fprintf(stderr, "Nie mo¿na utworzyæ okna.\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "ERROR: Failed to initialize GLAD!\n");
        return -1;
    }

    GLuint shader_programm = createShaderProgram("vertexShader.glsl", "fragmentShader.glsl");

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

    //snake.reset();

    TIME_NOW = TIME_LAST = glfwGetTime();
    TIME_DELTA = TIME_SUM = 0.0;

    while (!glfwWindowShouldClose(window)) {
        glClearColor(color_background[0], color_background[1], color_background[2], 1.0);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        keyFunctions(window);

        TIME_NOW = glfwGetTime();
        TIME_DELTA = TIME_NOW - TIME_LAST;
        TIME_LAST = TIME_NOW;
        TIME_SUM += TIME_DELTA;

        if (TIME_SUM >= 1.0 / SPEED) {
            snake.update();
            TIME_SUM = 0.0;
        }

        glUseProgram(shader_programm);
        glBindVertexArray(VAO);
        glUniform2f(glGetUniformLocation(shader_programm, "GRID_COUNT"), (GLfloat)GRID_COUNT_X, (GLfloat)GRID_COUNT_Y);

        // Draw Snake
        GLuint* snakePositions = snake.getPositions();
        for (GLuint i = 0; i < snake.getCounter(); i++) {
            glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), snakePositions[i]);

            if (i == 0) {
                glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_head[0], color_head[1], color_head[2]);
            }
            else {
                glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_tail[0], color_tail[1], color_tail[2]);
            }

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }

        // Draw Food
        glUniform1f(glGetUniformLocation(shader_programm, "POSITION"), snake.getFoodPosition());
        glUniform3f(glGetUniformLocation(shader_programm, "COLOR"), color_food[0], color_food[1], color_food[2]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    freeOpenGLProgram(window);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
